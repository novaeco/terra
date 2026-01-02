#pragma once
#include "esp_err.h"
#include "esp_lcd_panel_vendor.h"

esp_err_t display_jd9165_init(esp_lcd_panel_handle_t *out_panel);
