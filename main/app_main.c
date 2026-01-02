#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board_jc1060p470c.h"
#include "display_jd9165.h"
#include "lvgl_port.h"
#include "touch_gt911.h"

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
    touch_gt911_init(&touch, disp);

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
