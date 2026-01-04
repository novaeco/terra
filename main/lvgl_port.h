#pragma once
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"

esp_err_t lvgl_port_init(esp_lcd_panel_handle_t panel, lv_display_t **out_disp);
void lvgl_port_task_start(void);
