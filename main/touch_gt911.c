#include "touch_gt911.h"
#include "board_jc1060p470c.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include <string.h>
#include <stdbool.h>

static const char *TAG = "gt911";
static uint8_t i2c_addr = 0x5D; // will be updated by scan

static esp_err_t gt911_i2c_read(uint16_t reg, uint8_t *data, size_t len)
{
    uint8_t buf[2] = { reg >> 8, reg & 0xFF };
    if (i2c_master_write_read_device(I2C_NUM_0, i2c_addr, buf, 2, data, len, pdMS_TO_TICKS(100)) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t gt911_i2c_write(uint16_t reg, const uint8_t *data, size_t len)
{
    uint8_t tmp[2 + len];
    tmp[0] = reg >> 8;
    tmp[1] = reg & 0xFF;
    memcpy(&tmp[2], data, len);
    if (i2c_master_write_to_device(I2C_NUM_0, i2c_addr, tmp, len + 2, pdMS_TO_TICKS(100)) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t gt911_scan_i2c(void)
{
    ESP_LOGI(TAG, "I2C scan start (SCL=%d SDA=%d)", BOARD_PIN_TOUCH_SCL, BOARD_PIN_TOUCH_SDA);
    uint8_t found = 0;
    bool found_gt911 = false;
    for (int addr = 1; addr < 127; ++addr) {
        if (i2c_master_write_to_device(I2C_NUM_0, addr, NULL, 0, pdMS_TO_TICKS(10)) == ESP_OK) {
            ESP_LOGI(TAG, "I2C device at 0x%02X", addr);
            found++;
            if (addr == 0x5D || addr == 0x14) {
                found_gt911 = true;
            }
        }
    }
    if (!found) {
        ESP_LOGW(TAG, "I2C scan found nothing");
    }
    // Prefer GT911 addresses
    if (i2c_master_write_to_device(I2C_NUM_0, 0x5D, NULL, 0, pdMS_TO_TICKS(10)) == ESP_OK) {
        i2c_addr = 0x5D;
    } else if (i2c_master_write_to_device(I2C_NUM_0, 0x14, NULL, 0, pdMS_TO_TICKS(10)) == ESP_OK) {
        i2c_addr = 0x14;
    }
    ESP_LOGI(TAG, "GT911 address selected 0x%02X", i2c_addr);
    if (!found_gt911) {
        ESP_LOGE(TAG, "GT911 absent du bus I2C (adresses 0x5D/0x14 non répondues)");
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t gt911_init_chip(void)
{
    // Reset sequence
    gpio_set_level(BOARD_PIN_TOUCH_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(BOARD_PIN_TOUCH_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    uint8_t product_id[4] = {0};
    if (gt911_i2c_read(0x8140, product_id, 4) == ESP_OK) {
        ESP_LOGI(TAG, "GT911 ID: %c%c%c%c", product_id[0], product_id[1], product_id[2], product_id[3]);
    } else {
        ESP_LOGW(TAG, "GT911 ID read failed");
    }
    return ESP_OK;
}

static void lvgl_touch_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint8_t buf[8];
    if (gt911_i2c_read(0x814E, buf, 1) != ESP_OK) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    uint8_t points = buf[0] & 0x0F;
    if (points == 0) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    uint8_t point_buf[8];
    if (gt911_i2c_read(0x8150, point_buf, sizeof(point_buf)) != ESP_OK) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    uint16_t x = point_buf[1] << 8 | point_buf[0];
    uint16_t y = point_buf[3] << 8 | point_buf[2];
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PR;
    // clear status
    uint8_t clr = 0;
    gt911_i2c_write(0x814E, &clr, 1);
}

esp_err_t touch_gt911_init(touch_gt911_handle_t *out, lv_disp_t *disp)
{
    // I2C init
    i2c_config_t cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = BOARD_PIN_TOUCH_SDA,
        .scl_io_num = BOARD_PIN_TOUCH_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &cfg));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    // Touch pins
    gpio_set_direction(BOARD_PIN_TOUCH_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(BOARD_PIN_TOUCH_INT, GPIO_MODE_INPUT);

    if (gt911_scan_i2c() != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    gt911_init_chip();

    static lv_indev_data_t indev_drv;
    lv_fs_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_touch_cb;
    indev_drv.disp = disp;
    lv_indev_t *indev = lv_fs_drv_register(&indev_drv);
    if (out) {
        memset(out, 0, sizeof(*out));
        out->indev = indev;
        out->max_points = 5; // GT911 supporte jusqu'à 5 points
        out->initialized = true;
        uint8_t product_id[4] = {0};
        if (gt911_i2c_read(0x8140, product_id, sizeof(product_id)) == ESP_OK) {
            memcpy(out->product_id, product_id, sizeof(product_id));
            out->product_id[4] = '\0';
        } else {
            memcpy(out->product_id, "----", 4);
            out->product_id[4] = '\0';
        }
    }
    return ESP_OK;
}
