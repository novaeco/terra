#include "display_jd9165.h"
#include "board_jc1060p470c.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "jd9165";

// Timings from DTSI (MTK_JD9165BA_..._2lane.dtsi)
#define DOTCLK_MHZ 51.2
#define HSYNC 24
#define HBP 136
#define HFP 160
#define VSYNC 2
#define VBP 21
#define VFP 12

typedef struct {
    uint8_t cmd;
    const uint8_t *data;
    size_t data_bytes;
} jd9165_cmd_t;

static esp_err_t jd9165_send_cmds(esp_lcd_panel_io_handle_t io, const jd9165_cmd_t *cmds, size_t cmds_len)
{
    for (size_t i = 0; i < cmds_len; i++) {
        const jd9165_cmd_t *c = &cmds[i];
        if (c->data == NULL && c->data_bytes == 0) {
            vTaskDelay(pdMS_TO_TICKS(120));
            continue;
        }
        ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, c->cmd, c->data, c->data_bytes), TAG, "tx failed");
    }
    return ESP_OK;
}

esp_err_t display_jd9165_init(esp_lcd_panel_handle_t *out_panel)
{
    esp_lcd_dsi_bus_handle_t dsi_bus = NULL;
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    esp_lcd_dsi_bus_config_t bus_cfg = {
        .bus_id = 0,
        .num_data_lanes = 2, // from dtsi 0x0B=0x11 (2 lanes)
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 1000, // align with IDF EK79007 example for 1024x600
    };
    ESP_LOGI(TAG, "Config DSI lanes=%d bitrate=%.1fMbps", bus_cfg.num_data_lanes, (double)bus_cfg.lane_bit_rate_mbps);
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_cfg, &dsi_bus), TAG, "create DSI bus failed");

    esp_lcd_dbi_io_config_t io_cfg = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_dbi(dsi_bus, &io_cfg, &io_handle), TAG, "create DSI DBI IO failed");

    esp_lcd_dpi_panel_config_t dpi_cfg = {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = DOTCLK_MHZ,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .out_color_format = LCD_COLOR_FMT_RGB565,
        .num_fbs = 2,
        .video_timing = {
            .h_size = BOARD_LCD_H_RES,
            .v_size = BOARD_LCD_V_RES,
            .hsync_back_porch = HBP,
            .hsync_pulse_width = HSYNC,
            .hsync_front_porch = HFP,
            .vsync_back_porch = VBP,
            .vsync_pulse_width = VSYNC,
            .vsync_front_porch = VFP,
        },
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_dpi(dsi_bus, &dpi_cfg, &panel_handle), TAG, "create DPI panel failed");

    // Init sequence from dtsi
    const jd9165_cmd_t init_cmds[] = {
        { .cmd = 0x30, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0xF7, .data = (uint8_t[]){0x49,0x61,0x02,0x00}, .data_bytes = 4 },
        { .cmd = 0x30, .data = (uint8_t[]){0x01}, .data_bytes = 1 },
        { .cmd = 0x04, .data = (uint8_t[]){0x0C}, .data_bytes = 1 },
        { .cmd = 0x05, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x06, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x0B, .data = (uint8_t[]){0x11}, .data_bytes = 1 }, // 2 lanes per dtsi comment
        { .cmd = 0x17, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x20, .data = (uint8_t[]){0x04}, .data_bytes = 1 },
        { .cmd = 0x1F, .data = (uint8_t[]){0x05}, .data_bytes = 1 },
        { .cmd = 0x23, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x25, .data = (uint8_t[]){0x19}, .data_bytes = 1 },
        { .cmd = 0x28, .data = (uint8_t[]){0x18}, .data_bytes = 1 },
        { .cmd = 0x29, .data = (uint8_t[]){0x04}, .data_bytes = 1 },
        { .cmd = 0x2A, .data = (uint8_t[]){0x01}, .data_bytes = 1 },
        { .cmd = 0x2B, .data = (uint8_t[]){0x04}, .data_bytes = 1 },
        { .cmd = 0x2C, .data = (uint8_t[]){0x01}, .data_bytes = 1 },
        { .cmd = 0x30, .data = (uint8_t[]){0x02}, .data_bytes = 1 },
        { .cmd = 0x01, .data = (uint8_t[]){0x22}, .data_bytes = 1 },
        { .cmd = 0x03, .data = (uint8_t[]){0x12}, .data_bytes = 1 },
        { .cmd = 0x04, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x05, .data = (uint8_t[]){0x64}, .data_bytes = 1 },
        { .cmd = 0x0A, .data = (uint8_t[]){0x08}, .data_bytes = 1 },
        { .cmd = 0x0B, .data = (uint8_t[]){0x0A,0x1A,0x0B,0x0D,0x0D,0x11,0x10,0x06,0x08,0x1F,0x1D}, .data_bytes = 11 },
        { .cmd = 0x0C, .data = (uint8_t[]){0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D}, .data_bytes = 11 },
        { .cmd = 0x0D, .data = (uint8_t[]){0x16,0x1B,0x0B,0x0D,0x0D,0x11,0x10,0x07,0x09,0x1E,0x1C}, .data_bytes = 11 },
        { .cmd = 0x0E, .data = (uint8_t[]){0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D}, .data_bytes = 11 },
        { .cmd = 0x0F, .data = (uint8_t[]){0x16,0x1B,0x0D,0x0B,0x0D,0x11,0x10,0x1C,0x1E,0x09,0x07}, .data_bytes = 11 },
        { .cmd = 0x10, .data = (uint8_t[]){0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D}, .data_bytes = 11 },
        { .cmd = 0x11, .data = (uint8_t[]){0x0A,0x1A,0x0D,0x0B,0x0D,0x11,0x10,0x1D,0x1F,0x08,0x06}, .data_bytes = 11 },
        { .cmd = 0x12, .data = (uint8_t[]){0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D}, .data_bytes = 11 },
        { .cmd = 0x14, .data = (uint8_t[]){0x00,0x00,0x11,0x11}, .data_bytes = 4 },
        { .cmd = 0x18, .data = (uint8_t[]){0x99}, .data_bytes = 1 },
        { .cmd = 0x30, .data = (uint8_t[]){0x06}, .data_bytes = 1 },
        { .cmd = 0x12, .data = (uint8_t[]){0x36,0x2C,0x2E,0x3C,0x38,0x35,0x35,0x32,0x2E,0x1D,0x2B,0x21,0x16,0x29}, .data_bytes = 14 },
        { .cmd = 0x13, .data = (uint8_t[]){0x36,0x2C,0x2E,0x3C,0x38,0x35,0x35,0x32,0x2E,0x1D,0x2B,0x21,0x16,0x29}, .data_bytes = 14 },
        { .cmd = 0x30, .data = (uint8_t[]){0x0A}, .data_bytes = 1 },
        { .cmd = 0x02, .data = (uint8_t[]){0x4F}, .data_bytes = 1 },
        { .cmd = 0x0B, .data = (uint8_t[]){0x40}, .data_bytes = 1 },
        { .cmd = 0x12, .data = (uint8_t[]){0x3E}, .data_bytes = 1 },
        { .cmd = 0x13, .data = (uint8_t[]){0x78}, .data_bytes = 1 },
        { .cmd = 0x30, .data = (uint8_t[]){0x0D}, .data_bytes = 1 },
        { .cmd = 0x0D, .data = (uint8_t[]){0x04}, .data_bytes = 1 },
        { .cmd = 0x10, .data = (uint8_t[]){0x0C}, .data_bytes = 1 },
        { .cmd = 0x11, .data = (uint8_t[]){0x0C}, .data_bytes = 1 },
        { .cmd = 0x12, .data = (uint8_t[]){0x0C}, .data_bytes = 1 },
        { .cmd = 0x13, .data = (uint8_t[]){0x0C}, .data_bytes = 1 },
        { .cmd = 0x30, .data = (uint8_t[]){0x00}, .data_bytes = 1 },
        { .cmd = 0x11, .data = NULL, .data_bytes = 0 }, // sleep out
        { .cmd = 0x29, .data = NULL, .data_bytes = 0 }, // display on
    };

    ESP_LOGI(TAG, "Init JD9165 timings hs=%d hbp=%d hfp=%d vs=%d vbp=%d vfp=%d dotclk=%.1fMHz", HSYNC, HBP, HFP, VSYNC, VBP, VFP, DOTCLK_MHZ);

    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel_handle), TAG, "panel reset failed");
    ESP_RETURN_ON_ERROR(jd9165_send_cmds(io_handle, init_cmds, sizeof(init_cmds) / sizeof(init_cmds[0])), TAG, "panel init seq failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel_handle), TAG, "panel init failed");

    *out_panel = panel_handle;
    return ESP_OK;
}
