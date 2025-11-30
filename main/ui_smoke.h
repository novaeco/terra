#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_smoke_init(lv_display_t *disp);
void ui_smoke_boot_screen(void);
void ui_smoke_diag_screen(void);

#ifdef __cplusplus
}
#endif
