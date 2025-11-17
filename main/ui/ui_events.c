#include "ui_events.h"
#include "ui_screens.h"
#include "hal_sdcard.h"
#include "hal_can.h"
#include "hal_rs485.h"
#include "hal_ioexp_ch422g.h"
#include "ui_screens.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "ui_events";
static const char *test_text = "HELLO_RS485";

void ui_register_events(void)
{
    (void)TAG;
}

void ui_event_goto_home(lv_event_t *e)
{
    (void)e;
    ui_show_home();
}

void ui_event_goto_test(lv_event_t *e)
{
    (void)e;
    ui_show_test();
}

void ui_event_goto_system(lv_event_t *e)
{
    (void)e;
    ui_show_system();
}

void ui_event_sd_write(lv_event_t *e)
{
    (void)e;
    esp_err_t ret = hal_sdcard_write_test();
    if (ret == ESP_OK)
    {
        lv_label_set_text(label_sd_status, "SD: write OK");
    }
    else
    {
        lv_label_set_text(label_sd_status, "SD: write FAIL");
    }
}

void ui_event_sd_read(lv_event_t *e)
{
    (void)e;
    char buffer[128];
    esp_err_t ret = hal_sdcard_read_test(buffer, sizeof(buffer));
    if (ret == ESP_OK)
    {
        lv_label_set_text_fmt(label_sd_status, "SD read: %s", buffer);
    }
    else
    {
        lv_label_set_text(label_sd_status, "SD: read FAIL");
    }
}

void ui_event_can_send(lv_event_t *e)
{
    (void)e;
    esp_err_t ret = hal_can_send_test_frame();
    if (ret == ESP_OK)
    {
        lv_label_set_text(label_can_status, "CAN TX OK");
    }
    else
    {
        lv_label_set_text(label_can_status, "CAN TX ERR");
    }
}

void ui_event_rs485_send(lv_event_t *e)
{
    (void)e;
    esp_err_t ret = hal_rs485_send(test_text);
    char rx_buf[64];
    if (ret == ESP_OK)
    {
        lv_label_set_text(label_rs485_status, "RS485 TX OK");
        if (hal_rs485_receive(rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(50)) == ESP_OK)
        {
            lv_label_set_text_fmt(label_rs485_status, "RS485 RX: %s", rx_buf);
        }
    }
    else
    {
        lv_label_set_text(label_rs485_status, "RS485 TX ERR");
    }
}

void ui_event_ioexp_toggle0(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool level = lv_obj_has_state(sw, LV_STATE_CHECKED);
    esp_err_t ret = ch422g_set_pin(0, level);
    lv_label_set_text_fmt(label_ch422g_status, "P0=%d %s", level, (ret == ESP_OK) ? "OK" : "ERR");
}

void ui_event_ioexp_toggle1(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);
    bool level = lv_obj_has_state(sw, LV_STATE_CHECKED);
    esp_err_t ret = ch422g_set_pin(1, level);
    lv_label_set_text_fmt(label_ch422g_status, "P1=%d %s", level, (ret == ESP_OK) ? "OK" : "ERR");
}

void ui_event_touch_area(lv_event_t *e)
{
    lv_point_t point;
    lv_indev_get_point(lv_event_get_indev(e), &point);
    ui_move_touch_box(point);
}
