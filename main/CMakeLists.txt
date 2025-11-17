idf_component_register(SRCS
    "app_main.c"
    "hal_display.c"
    "hal_display.h"
    "hal_touch.c"
    "hal_touch.h"
    "hal_sdcard.c"
    "hal_sdcard.h"
    "hal_can.c"
    "hal_can.h"
    "hal_rs485.c"
    "hal_rs485.h"
    "hal_ioexp_ch422g.c"
    "hal_ioexp_ch422g.h"
    "ui/ui_init.c"
    "ui/ui_init.h"
    "ui/ui_screens.c"
    "ui/ui_screens.h"
    "ui/ui_events.c"
    "ui/ui_events.h"
    "ui/ui_theme.c"
    "ui/ui_theme.h"
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY C_STANDARD 11)

idf_component_get_property(lvgl_dir lvgl COMPONENT_DIR)
include_directories(${lvgl_dir})
