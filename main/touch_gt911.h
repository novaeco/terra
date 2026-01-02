#pragma once
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "lvgl.h"

typedef struct {
    lv_indev_t *indev;
} touch_gt911_handle_t;

esp_err_t touch_gt911_init(touch_gt911_handle_t *out, lv_disp_t *disp);
