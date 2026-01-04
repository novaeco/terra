#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board_jc1060p470c.h"
#include "display_jd9165.h"
#include "lvgl_port.h"
#include "touch_gt911.h"
#include <string.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_chip_info.h"
#include "esp_psram.h"
#include "esp_timer.h"
#include "esp_system.h"

static const char *TAG = "app";

typedef struct {
    lv_obj_t *diag_label;
    uint64_t start_us;
} ui_ctx_t;

static void show_toast(lv_obj_t *parent, const char *msg)
{
    lv_obj_t *toast = lv_label_create(parent);
    lv_label_set_text(toast, msg);
    lv_obj_set_style_bg_opa(toast, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_bg_color(toast, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(toast, 8, LV_PART_MAIN);
    lv_obj_set_style_radius(toast, 6, LV_PART_MAIN);
    lv_obj_set_style_text_color(toast, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(toast);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, toast);
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_time(&a, 1200);
    lv_anim_set_delay(&a, 500);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_set_ready_cb(&a, (lv_anim_ready_cb_t)lv_obj_del);
    lv_anim_start(&a);
}

static void action_btn_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *scr = lv_obj_get_screen(target);
    const char *text = NULL;
    lv_obj_t *label = lv_obj_get_child(target, 0);
    if (label) {
        text = lv_label_get_text(label);
    }
    show_toast(scr, text ? text : "Action");
}

static void diag_timer_cb(lv_timer_t *timer)
{
    ui_ctx_t *ctx = (ui_ctx_t *)timer->user_data;
    if (!ctx || !ctx->diag_label) {
        return;
    }
    uint64_t now_us = esp_timer_get_time();
    uint64_t up_ms = (now_us - ctx->start_us) / 1000ULL;
    uint32_t free_heap = esp_get_free_heap_size();
    char buf[96];
    snprintf(buf, sizeof(buf), "Uptime %llus | Heap libre %u", (unsigned long long)(up_ms / 1000ULL), (unsigned)free_heap);
    lv_label_set_text(ctx->diag_label, buf);
}

void app_main(void)
{
    ESP_LOGI(TAG, "JC1060P470C bring-up");
    board_init_pins();

    // Reset LCD sequence
    gpio_set_level(BOARD_PIN_LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(BOARD_PIN_LCD_STBYB, 1);
    gpio_set_level(BOARD_PIN_LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    esp_lcd_panel_handle_t panel = NULL;
    display_jd9165_init(&panel);

    board_backlight_on();

    lv_display_t *disp = NULL;
    lvgl_port_init(panel, &disp);
    lvgl_port_task_start();

    touch_gt911_handle_t touch;
    memset(&touch, 0, sizeof(touch));
    esp_err_t touch_err = touch_gt911_init(&touch, disp);
    if (touch_err != ESP_OK) {
        ESP_LOGE(TAG, "Initialisation GT911 échouée (%s)", esp_err_to_name(touch_err));
    }

    esp_chip_info_t chip_info = {0};
    esp_chip_info(&chip_info);
    const size_t psram_size = esp_psram_get_size();
    ESP_LOGI(TAG, "SoC: ESP32-P4, cœurs=%d, révision=%d, PSRAM=%u bytes", chip_info.cores, chip_info.revision, (unsigned int)psram_size);
    ESP_LOGI(TAG, "LCD JD9165 %dx%d (DSI 2 lanes) RST=%d STBYB=%d BL_EN=%d BL_PWM=%d", BOARD_LCD_H_RES, BOARD_LCD_V_RES, BOARD_PIN_LCD_RST, BOARD_PIN_LCD_STBYB, BOARD_PIN_BL_EN, BOARD_PIN_BL_PWM);
    ESP_LOGI(TAG, "Tactile GT911 bus I2C SDA=%d SCL=%d RST=%d INT=%d BOOT/strap=%d UART0 TX=%d RX=%d", BOARD_PIN_TOUCH_SDA, BOARD_PIN_TOUCH_SCL, BOARD_PIN_TOUCH_RST, BOARD_PIN_TOUCH_INT, BOARD_PIN_BOOT_MODE, BOARD_PIN_UART0_TX, BOARD_PIN_UART0_RX);
    if (touch.initialized) {
        ESP_LOGI(TAG, "GT911 détecté ID=%s, points max=%u", touch.product_id, touch.max_points);
    } else {
        ESP_LOGW(TAG, "GT911 non initialisé (aucune info ID disponible)");
    }

    // UI structurée
    lv_obj_t *scr = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xf5f5f5), LV_PART_MAIN);

    lv_obj_t *root = lv_obj_create(scr);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(root, 12, 0);
    lv_obj_set_style_pad_row(root, 10, 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

    lv_obj_t *header = lv_obj_create(root);
    lv_obj_set_width(header, LV_PCT(100));
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(header, 10, 0);
    lv_obj_set_style_pad_column(header, 10, 0);
    lv_obj_set_style_radius(header, 8, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_shadow_width(header, 8, 0);
    lv_obj_set_style_shadow_opa(header, LV_OPA_20, 0);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text_fmt(title, "JC1060P470C • LVGL");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    lv_obj_t *status = lv_label_create(header);
    lv_label_set_text_fmt(status, "LCD %dx%d | Touch %s", BOARD_LCD_H_RES, BOARD_LCD_V_RES, touch.initialized ? "OK" : "NOK");
    lv_obj_set_style_text_color(status, lv_color_hex(touch.initialized ? 0x2e7d32 : 0xd32f2f), 0);
    lv_obj_align(status, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *content = lv_obj_create(root);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(65));
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(content, 12, 0);
    lv_obj_set_style_pad_row(content, 10, 0);
    lv_obj_set_style_radius(content, 8, 0);
    lv_obj_set_style_bg_color(content, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_shadow_width(content, 8, 0);
    lv_obj_set_style_shadow_opa(content, LV_OPA_20, 0);

    lv_obj_t *card = lv_label_create(content);
    lv_label_set_text(card, "Écran de test tactile :\n- Glisser la liste\n- Manipuler le slider\n- Basculer le switch");
    lv_obj_set_style_text_color(card, lv_color_hex(0x424242), 0);

    lv_obj_t *list = lv_list_create(content);
    lv_obj_set_height(list, LV_PCT(45));
    lv_list_add_text(list, "Checklist");
    lv_list_add_btn(list, LV_SYMBOL_OK, "Contact tactile");
    lv_list_add_btn(list, LV_SYMBOL_UPLOAD, "DSI 2 voies");
    lv_list_add_btn(list, LV_SYMBOL_WIFI, "GPIO strap/boot");

    lv_obj_t *slider = lv_slider_create(content);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);

    lv_obj_t *switch_btn = lv_switch_create(content);
    lv_obj_add_state(switch_btn, LV_STATE_CHECKED);

    lv_obj_t *action_bar = lv_obj_create(root);
    lv_obj_set_width(action_bar, LV_PCT(100));
    lv_obj_set_flex_flow(action_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(action_bar, 8, 0);
    lv_obj_set_style_pad_column(action_bar, 10, 0);
    lv_obj_set_style_radius(action_bar, 8, 0);
    lv_obj_set_style_bg_color(action_bar, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_shadow_width(action_bar, 8, 0);
    lv_obj_set_style_shadow_opa(action_bar, LV_OPA_20, 0);

    lv_obj_t *btn_primary = lv_btn_create(action_bar);
    lv_obj_set_size(btn_primary, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_t *btn_primary_label = lv_label_create(btn_primary);
    lv_label_set_text(btn_primary_label, "Valider");
    lv_obj_center(btn_primary_label);
    lv_obj_add_event_cb(btn_primary, action_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_secondary = lv_btn_create(action_bar);
    lv_obj_set_size(btn_secondary, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_t *btn_secondary_label = lv_label_create(btn_secondary);
    lv_label_set_text(btn_secondary_label, "Diagnostics");
    lv_obj_center(btn_secondary_label);
    lv_obj_add_event_cb(btn_secondary, action_btn_event_cb, LV_EVENT_CLICKED, NULL);

    ui_ctx_t ui_ctx = {
        .diag_label = lv_label_create(action_bar),
        .start_us = esp_timer_get_time(),
    };
    lv_label_set_text(ui_ctx.diag_label, "Uptime 0s | Heap libre ...");
    lv_obj_set_style_text_color(ui_ctx.diag_label, lv_color_hex(0x616161), 0);

    lv_timer_t *diag_timer = lv_timer_create(diag_timer_cb, 1000, &ui_ctx);
    if (!diag_timer) {
        ESP_LOGW(TAG, "Impossible de créer le timer diag");
    }

    ESP_LOGI(TAG, "Init complete");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
