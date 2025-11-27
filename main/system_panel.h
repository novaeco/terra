#pragma once

#include <stdbool.h>

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

lv_obj_t *system_panel_create(void);
void system_panel_set_bus_status(bool i2c_ok, bool ch422g_ok, bool gt911_ok);
void system_panel_set_touch_status(bool touch_ok);

#ifdef __cplusplus
}
#endif
