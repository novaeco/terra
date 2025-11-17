#pragma once

#include "lvgl.h"

extern lv_obj_t *screen_home;
extern lv_obj_t *screen_test;
extern lv_obj_t *screen_system;
extern lv_obj_t *label_sd_status;
extern lv_obj_t *label_can_status;
extern lv_obj_t *label_rs485_status;
extern lv_obj_t *label_ch422g_status;
extern lv_obj_t *label_memory;
extern lv_obj_t *label_firmware;
extern lv_obj_t *label_sd_state;
extern lv_obj_t *label_can_state;
extern lv_obj_t *label_rs485_state;

void ui_create_home_screen(void);
void ui_create_test_screen(void);
void ui_create_system_screen(void);
void ui_show_home(void);
void ui_show_test(void);
void ui_show_system(void);
void ui_move_touch_box(lv_point_t point);
