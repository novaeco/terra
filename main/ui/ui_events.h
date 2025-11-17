#pragma once

#include "lvgl.h"

void ui_register_events(void);
void ui_event_goto_home(lv_event_t *e);
void ui_event_goto_test(lv_event_t *e);
void ui_event_goto_system(lv_event_t *e);
void ui_event_sd_write(lv_event_t *e);
void ui_event_sd_read(lv_event_t *e);
void ui_event_can_send(lv_event_t *e);
void ui_event_rs485_send(lv_event_t *e);
void ui_event_ioexp_toggle0(lv_event_t *e);
void ui_event_ioexp_toggle1(lv_event_t *e);
void ui_event_touch_area(lv_event_t *e);
