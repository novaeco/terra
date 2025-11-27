#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ui_manager_init(void);
esp_err_t ui_manager_init_step1_theme(void);
esp_err_t ui_manager_init_step2_screens(void);
esp_err_t ui_manager_init_step3_finalize(void);
void ui_manager_set_degraded(bool degraded);
void ui_manager_set_bus_status(bool i2c_ok, bool ch422g_ok, bool gt911_ok);
void ui_manager_set_touch_available(bool available);
bool ui_manager_touch_available(void);

#ifdef __cplusplus
}
#endif
