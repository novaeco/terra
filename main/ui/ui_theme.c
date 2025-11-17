#include "ui_theme.h"

void ui_theme_apply(void)
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x0E1B2E), 0);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xFFFFFF), 0);
}
