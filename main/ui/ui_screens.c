#include "ui_screens.h"
#include "ui_events.h"
#include "ui_theme.h"
#include "lvgl.h"

lv_obj_t *screen_home = NULL;
lv_obj_t *screen_test = NULL;
lv_obj_t *screen_system = NULL;

lv_obj_t *label_sd_status = NULL;
lv_obj_t *label_can_status = NULL;
lv_obj_t *label_rs485_status = NULL;
lv_obj_t *label_ch422g_status = NULL;
lv_obj_t *label_memory = NULL;
lv_obj_t *label_firmware = NULL;
lv_obj_t *label_sd_state = NULL;
lv_obj_t *label_can_state = NULL;
lv_obj_t *label_rs485_state = NULL;

static lv_obj_t *touch_box = NULL;

static void create_header(lv_obj_t *parent, const char *title)
{
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x0A2540), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t *label = lv_label_create(header);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label, lv_theme_get_font_large(label), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 20, 0);
}

void ui_create_home_screen(void)
{
    screen_home = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_home, lv_color_hex(0x0E1B2E), 0);
    create_header(screen_home, "ESP32-S3 Touch LCD 7B");

    lv_obj_t *container = lv_obj_create(screen_home);
    lv_obj_set_size(container, LV_PCT(90), LV_PCT(60));
    lv_obj_center(container);
    lv_obj_set_style_bg_opa(container, LV_OPA_20, 0);
    lv_obj_set_style_border_color(container, lv_color_hex(0x2D9CDB), 0);

    lv_obj_t *title = lv_label_create(container);
    lv_label_set_text(title, "Interface de base LVGL 9.2");
    lv_obj_set_style_text_font(title, lv_theme_get_font_large(title), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *btn_test = lv_btn_create(container);
    lv_obj_set_size(btn_test, 200, 60);
    lv_obj_align(btn_test, LV_ALIGN_CENTER, -120, 40);
    lv_obj_t *lbl_test = lv_label_create(btn_test);
    lv_label_set_text(lbl_test, "Test matériel");
    lv_obj_center(lbl_test);
    lv_obj_add_event_cb(btn_test, ui_event_goto_test, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_system = lv_btn_create(container);
    lv_obj_set_size(btn_system, 200, 60);
    lv_obj_align(btn_system, LV_ALIGN_CENTER, 120, 40);
    lv_obj_t *lbl_system = lv_label_create(btn_system);
    lv_label_set_text(lbl_system, "Système");
    lv_obj_center(lbl_system);
    lv_obj_add_event_cb(btn_system, ui_event_goto_system, LV_EVENT_CLICKED, NULL);
}

void ui_create_test_screen(void)
{
    screen_test = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_test, lv_color_hex(0x0E1B2E), 0);
    create_header(screen_test, "Tests matériels");
    lv_obj_add_event_cb(screen_test, ui_event_touch_area, LV_EVENT_PRESSING, NULL);

    lv_obj_t *btn_home = lv_btn_create(screen_test);
    lv_obj_align(btn_home, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_t *lbl_home = lv_label_create(btn_home);
    lv_label_set_text(lbl_home, "Accueil");
    lv_obj_center(lbl_home);
    lv_obj_add_event_cb(btn_home, ui_event_goto_home, LV_EVENT_CLICKED, NULL);

    lv_obj_t *grid = lv_obj_create(screen_test);
    static int32_t grid_cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_rows[] = {120, 120, 120, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(grid, grid_cols, grid_rows);
    lv_obj_set_size(grid, LV_PCT(95), LV_PCT(70));
    lv_obj_center(grid);
    lv_obj_set_style_bg_opa(grid, LV_OPA_0, 0);

    lv_obj_t *btn_sd_write = lv_btn_create(grid);
    lv_obj_set_grid_cell(btn_sd_write, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t *lbl_sd_write = lv_label_create(btn_sd_write);
    lv_label_set_text(lbl_sd_write, "Write test file");
    lv_obj_center(lbl_sd_write);
    lv_obj_add_event_cb(btn_sd_write, ui_event_sd_write, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_sd_read = lv_btn_create(grid);
    lv_obj_set_grid_cell(btn_sd_read, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t *lbl_sd_read = lv_label_create(btn_sd_read);
    lv_label_set_text(lbl_sd_read, "Read test file");
    lv_obj_center(lbl_sd_read);
    lv_obj_add_event_cb(btn_sd_read, ui_event_sd_read, LV_EVENT_CLICKED, NULL);

    label_sd_status = lv_label_create(grid);
    lv_obj_set_grid_cell(label_sd_status, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_label_set_text(label_sd_status, "SD: waiting...");

    lv_obj_t *btn_can = lv_btn_create(grid);
    lv_obj_set_grid_cell(btn_can, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_t *lbl_can = lv_label_create(btn_can);
    lv_label_set_text(lbl_can, "CAN send test");
    lv_obj_center(lbl_can);
    lv_obj_add_event_cb(btn_can, ui_event_can_send, LV_EVENT_CLICKED, NULL);

    label_can_status = lv_label_create(grid);
    lv_obj_set_grid_cell(label_can_status, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(label_can_status, "CAN: waiting...");

    lv_obj_t *btn_rs485 = lv_btn_create(grid);
    lv_obj_set_grid_cell(btn_rs485, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_t *lbl_rs485 = lv_label_create(btn_rs485);
    lv_label_set_text(lbl_rs485, "RS485 send");
    lv_obj_center(lbl_rs485);
    lv_obj_add_event_cb(btn_rs485, ui_event_rs485_send, LV_EVENT_CLICKED, NULL);

    label_rs485_status = lv_label_create(grid);
    lv_obj_set_grid_cell(label_rs485_status, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_label_set_text(label_rs485_status, "RS485: waiting...");

    lv_obj_t *io_container = lv_obj_create(screen_test);
    lv_obj_set_size(io_container, LV_PCT(80), 100);
    lv_obj_align(io_container, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_opa(io_container, LV_OPA_10, 0);

    lv_obj_t *switch_a = lv_switch_create(io_container);
    lv_obj_align(switch_a, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_add_event_cb(switch_a, ui_event_ioexp_toggle0, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_t *label_a = lv_label_create(io_container);
    lv_label_set_text(label_a, "CH422G P0");
    lv_obj_align_to(label_a, switch_a, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *switch_b = lv_switch_create(io_container);
    lv_obj_align(switch_b, LV_ALIGN_LEFT_MID, 200, 0);
    lv_obj_add_event_cb(switch_b, ui_event_ioexp_toggle1, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_t *label_b = lv_label_create(io_container);
    lv_label_set_text(label_b, "CH422G P1");
    lv_obj_align_to(label_b, switch_b, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    label_ch422g_status = lv_label_create(io_container);
    lv_obj_align(label_ch422g_status, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_label_set_text(label_ch422g_status, "IOEXP: --");

    touch_box = lv_obj_create(screen_test);
    lv_obj_set_size(touch_box, 120, 120);
    lv_obj_align(touch_box, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
    lv_obj_set_style_bg_color(touch_box, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_radius(touch_box, 12, 0);
}

void ui_create_system_screen(void)
{
    screen_system = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_system, lv_color_hex(0x0E1B2E), 0);
    create_header(screen_system, "Etat système");

    lv_obj_t *btn_home = lv_btn_create(screen_system);
    lv_obj_align(btn_home, LV_ALIGN_TOP_RIGHT, -20, 10);
    lv_obj_t *lbl_home = lv_label_create(btn_home);
    lv_label_set_text(lbl_home, "Accueil");
    lv_obj_center(lbl_home);
    lv_obj_add_event_cb(btn_home, ui_event_goto_home, LV_EVENT_CLICKED, NULL);

    lv_obj_t *list = lv_list_create(screen_system);
    lv_obj_set_size(list, LV_PCT(90), LV_PCT(70));
    lv_obj_center(list);

    label_firmware = lv_list_add_text(list, "Firmware: ");
    label_memory = lv_list_add_text(list, "RAM: --");
    label_sd_state = lv_list_add_text(list, "SD: --");
    label_can_state = lv_list_add_text(list, "CAN: --");
    label_rs485_state = lv_list_add_text(list, "RS485: --");
}

void ui_show_home(void)
{
    lv_scr_load(screen_home);
}

void ui_show_test(void)
{
    lv_scr_load(screen_test);
}

void ui_show_system(void)
{
    lv_scr_load(screen_system);
}

void ui_move_touch_box(lv_point_t point)
{
    if (!touch_box)
    {
        return;
    }
    lv_obj_set_style_bg_color(touch_box, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(touch_box, LV_ALIGN_TOP_LEFT, point.x - 60, point.y - 60);
}
