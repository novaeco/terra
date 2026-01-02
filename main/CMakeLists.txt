idf_component_register(
    SRCS
        app_main.c
        board_jc1060p470c.c
        display_jd9165.c
        touch_gt911.c
        lvgl_port.c
    INCLUDE_DIRS "."
    REQUIRES esp_lcd driver esp_lvgl_port
)
