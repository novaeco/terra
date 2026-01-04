#include "touch_gt911.h"
#include "board_jc1060p470c.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include <string.h>
#include <stdbool.h>

static const char *TAG = "gt911";
static uint8_t i2c_addr = 0x5D; // will be updated by scan
static i2c_master_bus_handle_t s_i2c_bus;
static i2c_master_dev_handle_t s_i2c_dev;

static esp_err_t gt911_i2c_read(uint16_t reg, uint8_t *data, size_t len)
{
    ESP_RETURN_ON_FALSE(s_i2c_dev, ESP_ERR_INVALID_STATE, TAG, "i2c device not ready");
    uint8_t buf[2] = { reg >> 8, reg & 0xFF };
    return i2c_master_transmit_receive(s_i2c_dev, buf, sizeof(buf), data, len, 100);
}

static esp_err_t gt911_i2c_write(uint16_t reg, const uint8_t *data, size_t len)
{
    ESP_RETURN_ON_FALSE(s_i2c_dev, ESP_ERR_INVALID_STATE, TAG, "i2c device not ready");
    uint8_t tmp[2 + len];
    tmp[0] = reg >> 8;
    tmp[1] = reg & 0xFF;
    memcpy(&tmp[2], data, len);
    return i2c_master_transmit(s_i2c_dev, tmp, len + 2, 100);
}

static esp_err_t gt911_scan_i2c(void)
{
    ESP_RETURN_ON_FALSE(s_i2c_bus, ESP_ERR_INVALID_STATE, TAG, "i2c bus not ready");
    ESP_LOGI(TAG, "I2C scan start (SCL=%d SDA=%d)", BOARD_PIN_TOUCH_SCL, BOARD_PIN_TOUCH_SDA);
    uint8_t found = 0;
    bool found_gt911 = false;
    for (int addr = 1; addr < 127; ++addr) {
        if (i2c_master_probe(s_i2c_bus, addr, 10) == ESP_OK) {
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
    if (i2c_master_probe(s_i2c_bus, 0x5D, 10) == ESP_OK) {
        i2c_addr = 0x5D;
    } else if (i2c_master_probe(s_i2c_bus, 0x14, 10) == ESP_OK) {
        i2c_addr = 0x14;
    }
    if (s_i2c_dev) {
        i2c_master_bus_rm_device(s_i2c_dev);
        s_i2c_dev = NULL;
    }
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = 400000,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(s_i2c_bus, &dev_cfg, &s_i2c_dev), TAG, "add device failed");

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
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    uint8_t points = buf[0] & 0x0F;
    if (points == 0) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    uint8_t point_buf[8];
    if (gt911_i2c_read(0x8150, point_buf, sizeof(point_buf)) != ESP_OK) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    uint16_t x = point_buf[1] << 8 | point_buf[0];
    uint16_t y = point_buf[3] << 8 | point_buf[2];
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
    // clear status
    uint8_t clr = 0;
    gt911_i2c_write(0x814E, &clr, 1);
}

esp_err_t touch_gt911_init(touch_gt911_handle_t *out, lv_display_t *disp)
{
    if (!s_i2c_bus) {
        i2c_master_bus_config_t bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = BOARD_PIN_TOUCH_SDA,
            .scl_io_num = BOARD_PIN_TOUCH_SCL,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_cfg, &s_i2c_bus), TAG, "bus init failed");
    }

    // Touch pins
    gpio_set_direction(BOARD_PIN_TOUCH_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(BOARD_PIN_TOUCH_INT, GPIO_MODE_INPUT);

    if (gt911_scan_i2c() != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    gt911_init_chip();

    lv_indev_t *indev = lv_indev_create();
    if (!indev) {
        ESP_LOGE(TAG, "Impossible de créer l'indev LVGL");
        return ESP_ERR_NO_MEM;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_touch_cb);
    lv_indev_set_display(indev, disp);
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
