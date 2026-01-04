#pragma once
#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "lvgl.h"

typedef struct {
    lv_indev_t *indev;
    char product_id[5];
    uint8_t max_points;
    bool initialized;
} touch_gt911_handle_t;

esp_err_t touch_gt911_init(touch_gt911_handle_t *out, lv_display_t *disp);
