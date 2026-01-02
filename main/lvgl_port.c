#include "lvgl_port.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lvgl_port.h"

static const char *TAG = "lvgl";
static lv_disp_t *s_disp = NULL;

static void lvgl_tick_cb(void *arg)
{
    lv_tick_inc(5);
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)drv->user_data;
    int32_t x1 = area->x1, y1 = area->y1;
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    esp_lcd_panel_draw_bitmap(panel, x1, y1, x1 + w, y1 + h, color_map);
    lv_disp_flush_ready(drv);
}

esp_err_t lvgl_port_init(esp_lcd_panel_handle_t panel, lv_disp_t **out_disp)
{
    const size_t buf_pixels = 1024 * 50; // ~100KB double buffer si 16bpp
    lv_color_t *buf1 = heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    lv_color_t *buf2 = heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf1 || !buf2) {
        ESP_LOGE(TAG, "LVGL buffers alloc failed");
        return ESP_ERR_NO_MEM;
    }
    lv_init();
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_pixels);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 1024;
    disp_drv.ver_res = 600;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.user_data = panel;
    s_disp = lv_disp_drv_register(&disp_drv);
    if (out_disp) {
        *out_disp = s_disp;
    }

    // Tick source
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = lvgl_tick_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 5000));
    ESP_LOGI(TAG, "LVGL init ok, buffers=%zu px", buf_pixels);
    return ESP_OK;
}

static void lvgl_task(void *arg)
{
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void lvgl_port_task_start(void)
{
    xTaskCreatePinnedToCore(lvgl_task, "lvgl", 4096, NULL, 5, NULL, 0);
}
