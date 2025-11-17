#pragma once

#include "lvgl.h"

void ui_init(lv_display_t *display, lv_indev_t *indev);
void ui_update_system_info(void);
void ui_set_sd_status(bool ok, const char *message);
void ui_set_can_status(bool ok);
void ui_set_rs485_status(bool ok, const char *rx_msg);
void ui_set_ioexp_status(uint8_t pin, bool level);
