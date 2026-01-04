#pragma once
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"

esp_err_t app_lvgl_port_init(esp_lcd_panel_handle_t panel, lv_display_t **out_disp);
void app_lvgl_port_task_start(void);
