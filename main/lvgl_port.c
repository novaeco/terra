#include "lvgl_port.h"
#include "board_jc1060p470c.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lvgl_port.h"

static const char *TAG = "lvgl";
static lv_display_t *s_disp = NULL;

static void lvgl_tick_cb(void *arg)
{
    lv_tick_inc(5);
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
}

static bool lvgl_flush_ready_cb(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
}

esp_err_t lvgl_port_init(esp_lcd_panel_handle_t panel, lv_display_t **out_disp)
{
    const size_t buf_pixels = BOARD_LCD_H_RES * 50; // ~100KB double buffer si 16bpp
    lv_color_t *buf1 = heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    lv_color_t *buf2 = heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(buf1 && buf2, ESP_ERR_NO_MEM, TAG, "LVGL buffers alloc failed");

    lv_init();

    lv_display_t *disp = lv_display_create(BOARD_LCD_H_RES, BOARD_LCD_V_RES);
    lv_display_set_user_data(disp, panel);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_buffers(disp, buf1, buf2, buf_pixels * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = lvgl_flush_ready_cb,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_dpi_panel_register_event_callbacks(panel, &cbs, disp), TAG, "register panel cb failed");

    s_disp = disp;
    if (out_disp) {
        *out_disp = s_disp;
    }

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = lvgl_tick_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t periodic_timer;
    ESP_RETURN_ON_ERROR(esp_timer_create(&periodic_timer_args, &periodic_timer), TAG, "timer create failed");
    ESP_RETURN_ON_ERROR(esp_timer_start_periodic(periodic_timer, 5000), TAG, "timer start failed");
    ESP_LOGI(TAG, "LVGL init ok, buffers=%zu px", buf_pixels);
    return ESP_OK;
}

static void lvgl_task(void *arg)
{
    while (1) {
        uint32_t wait_ms = lv_timer_handler();
        if (wait_ms < 5) {
            wait_ms = 5;
        }
        vTaskDelay(pdMS_TO_TICKS(wait_ms));
    }
}

void lvgl_port_task_start(void)
{
    xTaskCreatePinnedToCore(lvgl_task, "lvgl", 4096, NULL, 5, NULL, 0);
}
