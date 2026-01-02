#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board_jc1060p470c.h"
#include "display_jd9165.h"
#include "lvgl_port.h"
#include "touch_gt911.h"
#include <string.h>
#include "esp_err.h"
#include "esp_chip_info.h"
#include "esp_psram.h"

static const char *TAG = "app";

static void anim_set_x_cb(void *obj, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)obj, v);
}

void app_main(void)
{
    ESP_LOGI(TAG, "JC1060P470C bring-up");
    board_init_pins();

    // Reset LCD sequence
    gpio_set_level(BOARD_PIN_LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(BOARD_PIN_LCD_STBYB, 1);
    gpio_set_level(BOARD_PIN_LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    esp_lcd_panel_handle_t panel = NULL;
    display_jd9165_init(&panel);

    board_backlight_on();

    lv_disp_t *disp = NULL;
    lvgl_port_init(panel, &disp);
    lvgl_port_task_start();

    touch_gt911_handle_t touch;
    memset(&touch, 0, sizeof(touch));
    esp_err_t touch_err = touch_gt911_init(&touch, disp);
    if (touch_err != ESP_OK) {
        ESP_LOGE(TAG, "Initialisation GT911 échouée (%s)", esp_err_to_name(touch_err));
    }

    esp_chip_info_t chip_info = {0};
    esp_chip_info(&chip_info);
    const size_t psram_size = esp_psram_get_size();
    ESP_LOGI(TAG, "SoC: ESP32-P4, cœurs=%d, révision=%d, PSRAM=%u bytes", chip_info.cores, chip_info.revision, (unsigned int)psram_size);
    ESP_LOGI(TAG, "LCD JD9165 %dx%d (DSI 2 lanes) RST=%d STBYB=%d BL_EN=%d BL_PWM=%d", BOARD_LCD_H_RES, BOARD_LCD_V_RES, BOARD_PIN_LCD_RST, BOARD_PIN_LCD_STBYB, BOARD_PIN_BL_EN, BOARD_PIN_BL_PWM);
    ESP_LOGI(TAG, "Tactile GT911 bus I2C SDA=%d SCL=%d RST=%d INT=%d BOOT/strap=%d UART0 TX=%d RX=%d", BOARD_PIN_TOUCH_SDA, BOARD_PIN_TOUCH_SCL, BOARD_PIN_TOUCH_RST, BOARD_PIN_TOUCH_INT, BOARD_PIN_BOOT_MODE, BOARD_PIN_UART0_TX, BOARD_PIN_UART0_RX);
    if (touch.initialized) {
        ESP_LOGI(TAG, "GT911 détecté ID=%s, points max=%u", touch.product_id, touch.max_points);
    } else {
        ESP_LOGW(TAG, "GT911 non initialisé (aucune info ID disponible)");
    }

    // Demo UI
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_center(btn);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "JC1060P470C");
    lv_obj_center(label);

    // Simple animation
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, btn);
    lv_anim_set_values(&a, 20, 200);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, anim_set_x_cb);
    lv_anim_start(&a);

    ESP_LOGI(TAG, "Init complete");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
