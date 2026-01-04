idf_component_register(
    SRCS
        app_main.c
        board_jc1060p470c.c
        display_jd9165.c
        touch_gt911.c
        app_lvgl_port.c
    INCLUDE_DIRS "."
    REQUIRES esp_lcd esp_driver_gpio esp_driver_ledc esp_driver_i2c esp_lvgl_port
    PRIV_REQUIRES esp_psram esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE=1)
