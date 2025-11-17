#pragma once

#include "esp_err.h"
#include "lvgl.h"

#define LCD_H_RES 1024
#define LCD_V_RES 600

// RGB data pins and control pins based on Waveshare ESP32-S3-Touch-LCD-7B
#define LCD_PIN_NUM_HSYNC 46
#define LCD_PIN_NUM_VSYNC 3
#define LCD_PIN_NUM_DE 5
#define LCD_PIN_NUM_PCLK 7
#define LCD_PIN_NUM_DISP_EN 4
#define LCD_PIN_NUM_BACKLIGHT 45

#define LCD_PIN_NUM_DATA0 14
#define LCD_PIN_NUM_DATA1 38
#define LCD_PIN_NUM_DATA2 18
#define LCD_PIN_NUM_DATA3 17
#define LCD_PIN_NUM_DATA4 16
#define LCD_PIN_NUM_DATA5 15
#define LCD_PIN_NUM_DATA6 8
#define LCD_PIN_NUM_DATA7 6
#define LCD_PIN_NUM_DATA8 41
#define LCD_PIN_NUM_DATA9 40
#define LCD_PIN_NUM_DATA10 39
#define LCD_PIN_NUM_DATA11 0
#define LCD_PIN_NUM_DATA12 48
#define LCD_PIN_NUM_DATA13 47
#define LCD_PIN_NUM_DATA14 21
#define LCD_PIN_NUM_DATA15 20

esp_err_t hal_display_init(lv_display_t **out_display);
void hal_display_flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);

