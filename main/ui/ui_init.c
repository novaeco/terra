#include "ui_init.h"
#include "ui_screens.h"
#include "ui_theme.h"
#include "ui_events.h"
#include "hal_sdcard.h"
#include "hal_can.h"
#include "hal_rs485.h"
#include "hal_ioexp_ch422g.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include <stdio.h>

#define FW_VERSION "v1.0.0"

static lv_display_t *ui_display = NULL;
static lv_indev_t *ui_indev = NULL;

void ui_init(lv_display_t *display, lv_indev_t *indev)
{
    ui_display = display;
    ui_indev = indev;
    ui_theme_apply();
    ui_create_home_screen();
    ui_create_test_screen();
    ui_create_system_screen();
    ui_register_events();
    ui_show_home();
    ui_update_system_info();
}

void ui_update_system_info(void)
{
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    uint32_t free_internal = info.total_free_bytes;
    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    uint32_t free_psram = info.total_free_bytes;

    lv_label_set_text_fmt(label_memory, "RAM libre: %lu / PSRAM libre: %lu", (unsigned long)free_internal, (unsigned long)free_psram);
    lv_label_set_text_fmt(label_firmware, "Firmware: %s", FW_VERSION);
    lv_label_set_text_fmt(label_sd_state, "SD: %s", hal_sdcard_is_mounted() ? "montée" : "non montée");
    lv_label_set_text_fmt(label_can_state, "CAN: %s", hal_can_is_started() ? "actif" : "stoppé");
    lv_label_set_text_fmt(label_rs485_state, "RS485: %s", hal_rs485_is_ready() ? "actif" : "stoppé");
}

void ui_set_sd_status(bool ok, const char *message)
{
    lv_label_set_text_fmt(label_sd_status, "SD: %s (%s)", ok ? "OK" : "ERR", message);
}

void ui_set_can_status(bool ok)
{
    lv_label_set_text(label_can_status, ok ? "CAN TX OK" : "CAN TX ERR");
}

void ui_set_rs485_status(bool ok, const char *rx_msg)
{
    if (rx_msg)
    {
        lv_label_set_text_fmt(label_rs485_status, "RS485 %s: %s", ok ? "OK" : "ERR", rx_msg);
    }
    else
    {
        lv_label_set_text(label_rs485_status, ok ? "RS485 OK" : "RS485 ERR");
    }
}

void ui_set_ioexp_status(uint8_t pin, bool level)
{
    lv_label_set_text_fmt(label_ch422g_status, "P%d=%d", pin, level);
}
