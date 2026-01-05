/**
 * @file main.c
 * @brief ESP32-P4 LVGL Smart Panel for GUITION JC1060P470C_I_W_Y (7-inch)
 *
 * Features:
 *   - Multi-page UI with navigation
 *   - Status bar with WiFi, Bluetooth, Date, Time
 *   - SD Card mounted with image loading support
 *   - PNG/JPEG decoder for LVGL
 *   - Touch support (GT911)
 *   - WiFi/BLE via ESP32-C6 (esp_hosted)
 *   - Camera support (OV02C10)
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// GPIO and power control
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_ldo_regulator.h"

// I2C for touch
#include "driver/i2c_master.h"

// SD Card
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

// MIPI-DSI and LCD panel
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7701.h"
#include "esp_lcd_types.h"

// Touch driver
#include "esp_lcd_touch_gt911.h"

// WiFi via ESP32-C6 (esp_hosted)
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

// LVGL port
#include "esp_lvgl_port.h"
#include "lvgl.h"

// Bluetooth via ESP32-C6 (esp_hosted) - conditionally included
#if CONFIG_BT_ENABLED
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_hosted_bluedroid.h"
#endif // CONFIG_BT_ENABLED

// ESP-Hosted (always needed for WiFi and OTA)
#include "esp_hosted.h"

// ESP32-C6 embedded firmware for OTA
extern const uint8_t
    slave_fw_start[] asm("_binary_network_adapter_esp32c6_bin_start");
extern const uint8_t
    slave_fw_end[] asm("_binary_network_adapter_esp32c6_bin_end");

static const char *TAG = "SMART_PANEL_7";
static const char *WIFI_TAG = "WIFI";

// ====================================================================================
// HARDWARE CONFIGURATION - JC1060P470C (7-inch 1024x600 JD9165BA)
// ====================================================================================

// Display resolution (7-inch IPS)
#define LCD_H_RES 1024
#define LCD_V_RES 600

// GPIO Configuration (may need adjustment based on schematic)
#define LCD_RST_GPIO 5
#define LCD_BL_GPIO 23

// Touch I2C
#define TOUCH_I2C_SDA 7
#define TOUCH_I2C_SCL 8
#define TOUCH_I2C_FREQ_HZ 400000

// MIPI-DSI Configuration for JD9165BA (2 lanes)
#define DSI_LANE_NUM 2
#define DSI_LANE_BITRATE                                                       \
  800 // Increased for 1024x600@60Hz (needs ~500 Mbps + margin)
#define DPI_CLOCK_MHZ 52 // ~51.2 MHz as per datasheet

#define DSI_PHY_LDO_CHANNEL 3
#define DSI_PHY_VOLTAGE_MV 2500

#define BL_LEDC_TIMER LEDC_TIMER_0
#define BL_LEDC_CHANNEL LEDC_CHANNEL_0
#define BL_PWM_FREQ 5000

// SD Card GPIOs (from JC4880P443C schematic)
#define SD_CMD_GPIO 44
#define SD_CLK_GPIO 43
#define SD_D0_GPIO 39
#define SD_D1_GPIO 40
#define SD_D2_GPIO 41
#define SD_D3_GPIO 42

#define SD_MOUNT_POINT "/sdcard"

// ====================================================================================
// JD9165BA INIT COMMANDS (7-inch 1024x600 panel)
// Based on MTK_JD9165BA_HKC7.0_IPS datasheet
// VCC=1.8V/3.3V, AVDD=11V, VGH=20V, VGL=-7V, VCOM=3.6V
// 2 MIPI lanes, 1+2Dot inversion
// ====================================================================================

static const st7701_lcd_init_cmd_t jd9165ba_lcd_cmds[] = {
    // Page select 0
    {0x30, (uint8_t[]){0x00}, 1, 0},
    {0xF7, (uint8_t[]){0x49, 0x61, 0x02, 0x00}, 4, 0},
    // Page select 1
    {0x30, (uint8_t[]){0x01}, 1, 0},
    {0x04, (uint8_t[]){0x0C}, 1, 0},
    {0x05, (uint8_t[]){0x00}, 1, 0},
    {0x06, (uint8_t[]){0x00}, 1, 0},
    {0x0B, (uint8_t[]){0x11}, 1, 0}, // 0x11 = 2 lanes
    {0x17, (uint8_t[]){0x00}, 1, 0},
    {0x20, (uint8_t[]){0x04}, 1, 0}, // r_lansel_sel_reg=1
    {0x1F, (uint8_t[]){0x05}, 1, 0}, // hs_settle time
    {0x23, (uint8_t[]){0x00}, 1, 0}, // close gas
    {0x25, (uint8_t[]){0x19}, 1, 0},
    {0x28, (uint8_t[]){0x18}, 1, 0},
    {0x29, (uint8_t[]){0x04}, 1, 0}, // revcom
    {0x2A, (uint8_t[]){0x01}, 1, 0}, // revcom
    {0x2B, (uint8_t[]){0x04}, 1, 0}, // vcom
    {0x2C, (uint8_t[]){0x01}, 1, 0}, // vcom
    // Page select 2
    {0x30, (uint8_t[]){0x02}, 1, 0},
    {0x01, (uint8_t[]){0x22}, 1, 0},
    {0x03, (uint8_t[]){0x12}, 1, 0},
    {0x04, (uint8_t[]){0x00}, 1, 0},
    {0x05, (uint8_t[]){0x64}, 1, 0},
    {0x0A, (uint8_t[]){0x08}, 1, 0},
    {0x0B,
     (uint8_t[]){0x0A, 0x1A, 0x0B, 0x0D, 0x0D, 0x11, 0x10, 0x06, 0x08, 0x1F,
                 0x1D},
     11, 0},
    {0x0C,
     (uint8_t[]){0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
                 0x0D},
     11, 0},
    {0x0D,
     (uint8_t[]){0x16, 0x1B, 0x0B, 0x0D, 0x0D, 0x11, 0x10, 0x07, 0x09, 0x1E,
                 0x1C},
     11, 0},
    {0x0E,
     (uint8_t[]){0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
                 0x0D},
     11, 0},
    {0x0F,
     (uint8_t[]){0x16, 0x1B, 0x0D, 0x0B, 0x0D, 0x11, 0x10, 0x1C, 0x1E, 0x09,
                 0x07},
     11, 0},
    {0x10,
     (uint8_t[]){0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
                 0x0D},
     11, 0},
    {0x11,
     (uint8_t[]){0x0A, 0x1A, 0x0D, 0x0B, 0x0D, 0x11, 0x10, 0x1D, 0x1F, 0x08,
                 0x06},
     11, 0},
    {0x12,
     (uint8_t[]){0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
                 0x0D},
     11, 0},
    {0x14, (uint8_t[]){0x00, 0x00, 0x11, 0x11}, 4, 0}, // CKV_OFF
    {0x18, (uint8_t[]){0x99}, 1, 0},
    // Page select 6 - Gamma
    {0x30, (uint8_t[]){0x06}, 1, 0},
    {0x12,
     (uint8_t[]){0x36, 0x2C, 0x2E, 0x3C, 0x38, 0x35, 0x35, 0x32, 0x2E, 0x1D,
                 0x2B, 0x21, 0x16, 0x29},
     14, 0},
    {0x13,
     (uint8_t[]){0x36, 0x2C, 0x2E, 0x3C, 0x38, 0x35, 0x35, 0x32, 0x2E, 0x1D,
                 0x2B, 0x21, 0x16, 0x29},
     14, 0},
    // Page select A
    {0x30, (uint8_t[]){0x0A}, 1, 0},
    {0x02, (uint8_t[]){0x4F}, 1, 0},
    {0x0B, (uint8_t[]){0x40}, 1, 0},
    {0x12, (uint8_t[]){0x3E}, 1, 0},
    {0x13, (uint8_t[]){0x78}, 1, 0},
    // Page select D
    {0x30, (uint8_t[]){0x0D}, 1, 0},
    {0x0D, (uint8_t[]){0x04}, 1, 0},
    {0x10, (uint8_t[]){0x0C}, 1, 0},
    {0x11, (uint8_t[]){0x0C}, 1, 0},
    {0x12, (uint8_t[]){0x0C}, 1, 0},
    {0x13, (uint8_t[]){0x0C}, 1, 0},
    // Page select 0
    {0x30, (uint8_t[]){0x00}, 1, 0},
    // Sleep Out
    {0x11, (uint8_t[]){0x00}, 0, 120},
    // Display On
    {0x29, (uint8_t[]){0x00}, 0, 20},
};

// ====================================================================================
// GLOBAL HANDLES AND STATE
// ====================================================================================

static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;
static lv_display_t *main_display = NULL;
static sdmmc_card_t *sd_card = NULL;
static bool sd_mounted = false;

// State
static uint8_t current_brightness = 100;
static bool wifi_enabled = false;
static bool wifi_connected = false;
static bool bluetooth_enabled = true;

// WiFi State
static char wifi_ssid[33] = "";
static char wifi_ip[16] = "0.0.0.0";
static esp_netif_t *sta_netif = NULL;

// WiFi Credentials (can be moved to NVS)
#define WIFI_SSID_DEFAULT "YourSSID"
#define WIFI_PASS_DEFAULT "YourPassword"

// Pages
static lv_obj_t *page_home = NULL;
static lv_obj_t *page_settings = NULL;
static lv_obj_t *page_wifi = NULL;
static lv_obj_t *page_bluetooth = NULL;

// WiFi Scan Results
#define WIFI_SCAN_MAX_AP 20
static wifi_ap_record_t wifi_scan_results[WIFI_SCAN_MAX_AP];
static uint16_t wifi_scan_count = 0;
static char wifi_selected_ssid[33] = "";
static char wifi_password_input[65] = "";

// UI Elements
static lv_obj_t *label_time = NULL;
static lv_obj_t *label_date = NULL;
static lv_obj_t *icon_wifi = NULL;
static lv_obj_t *icon_bluetooth = NULL;
static lv_obj_t *logo_img = NULL;
static lv_obj_t *slider_brightness = NULL;
static lv_obj_t *sd_status_label = NULL;

// WiFi Page UI Elements
static lv_obj_t *wifi_list = NULL;
static lv_obj_t *wifi_keyboard = NULL;
static lv_obj_t *wifi_password_ta = NULL;
static lv_obj_t *wifi_status_label = NULL;
static lv_obj_t *wifi_ssid_label = NULL;
static lv_obj_t *wifi_pwd_container = NULL;

// Bluetooth Page UI Elements
static lv_obj_t *bt_list = NULL;
static lv_obj_t *bt_status_label = NULL;
static lv_obj_t *bt_device_label = NULL;

// ====================================================================================
// COLOR THEME
// ====================================================================================

#define COLOR_BG_DARK lv_color_hex(0x0F0F0F)
#define COLOR_BG_CARD lv_color_hex(0x1C1C1E)
#define COLOR_ACCENT lv_color_hex(0x2C2C2E)
#define COLOR_PRIMARY lv_color_hex(0x007AFF)
#define COLOR_SUCCESS lv_color_hex(0x30D158)
#define COLOR_WARNING lv_color_hex(0xFF9F0A)
#define COLOR_DANGER lv_color_hex(0xFF453A)
#define COLOR_TEXT lv_color_hex(0xFFFFFF)
#define COLOR_TEXT_DIM lv_color_hex(0x8E8E93)
#define COLOR_BORDER lv_color_hex(0x38383A)
#define COLOR_HEADER lv_color_hex(0x1C1C1E)

// ====================================================================================
// SD CARD FUNCTIONS
// ====================================================================================

static esp_err_t sd_card_init(void) {
  ESP_LOGI(TAG, "Initializing SD card...");

  // Use SDMMC Slot 0 for SD card (Slot 1 is used by esp_hosted for C6)
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.slot = SDMMC_HOST_SLOT_0;          // Explicitly use Slot 0
  host.max_freq_khz = SDMMC_FREQ_DEFAULT; // Lower freq for stability

  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 4; // 4-bit mode
  slot_config.clk = SD_CLK_GPIO;
  slot_config.cmd = SD_CMD_GPIO;
  slot_config.d0 = SD_D0_GPIO;
  slot_config.d1 = SD_D1_GPIO;
  slot_config.d2 = SD_D2_GPIO;
  slot_config.d3 = SD_D3_GPIO;
  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot_config,
                                          &mount_config, &sd_card);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to mount SD card: %s", esp_err_to_name(ret));
    sd_mounted = false;
    return ret;
  }

  sdmmc_card_print_info(stdout, sd_card);
  sd_mounted = true;
  ESP_LOGI(TAG, "SD card mounted at %s", SD_MOUNT_POINT);

  // List files in /sdcard/imgs
  DIR *dir = opendir(SD_MOUNT_POINT "/imgs");
  if (dir) {
    ESP_LOGI(TAG, "Files in %s/imgs:", SD_MOUNT_POINT);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      ESP_LOGI(TAG, "  - %s", entry->d_name);
    }
    closedir(dir);
  }

  return ESP_OK;
}

// ====================================================================================
// WIFI FUNCTIONS (via ESP32-C6 co-processor)
// ====================================================================================

// Forward declaration for NVS functions (defined later)
static esp_err_t wifi_save_credentials(const char *ssid, const char *password);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
    case WIFI_EVENT_STA_START:
      ESP_LOGI(WIFI_TAG, "WiFi STA started, connecting...");
      esp_wifi_connect();
      break;
    case WIFI_EVENT_STA_CONNECTED:
      ESP_LOGI(WIFI_TAG, "Connected to AP!");
      wifi_event_sta_connected_t *conn_event =
          (wifi_event_sta_connected_t *)event_data;
      snprintf(wifi_ssid, sizeof(wifi_ssid), "%s", conn_event->ssid);
      break;
    case WIFI_EVENT_STA_DISCONNECTED: {
      wifi_event_sta_disconnected_t *disc_event =
          (wifi_event_sta_disconnected_t *)event_data;
      ESP_LOGW(WIFI_TAG, "Disconnected from AP! Reason: %d",
               disc_event->reason);

      // Log common reasons for debugging
      switch (disc_event->reason) {
      case 2:
        ESP_LOGW(WIFI_TAG, "  -> AUTH_EXPIRE");
        break;
      case 15:
        ESP_LOGW(WIFI_TAG, "  -> 4WAY_HANDSHAKE_TIMEOUT (wrong password?)");
        break;
      case 201:
        ESP_LOGW(WIFI_TAG, "  -> NO_AP_FOUND");
        break;
      case 202:
        ESP_LOGW(WIFI_TAG, "  -> AUTH_FAIL (wrong password)");
        break;
      case 203:
        ESP_LOGW(WIFI_TAG, "  -> ASSOC_FAIL");
        break;
      default:
        ESP_LOGW(WIFI_TAG, "  -> Unknown reason");
        break;
      }

      wifi_connected = false;
      memset(wifi_ssid, 0, sizeof(wifi_ssid));
      memset(wifi_ip, 0, sizeof(wifi_ip));

      // Update UI to show failure (if we have lock)
      if (lvgl_port_lock(10)) {
        if (wifi_status_label) {
          char status_msg[64];
          snprintf(status_msg, sizeof(status_msg),
                   "Connection failed (reason: %d)", disc_event->reason);
          lv_label_set_text(wifi_status_label, status_msg);
        }
        lvgl_port_unlock();
      }

      // Only retry a few times for password errors
      static int retry_count = 0;
      if (disc_event->reason == 15 || disc_event->reason == 202) {
        // Wrong password - don't retry infinitely
        if (retry_count < 2) {
          retry_count++;
          ESP_LOGI(WIFI_TAG, "Retrying connection (attempt %d)...",
                   retry_count);
          esp_wifi_connect();
        } else {
          ESP_LOGE(WIFI_TAG, "Authentication failed - check password!");
          retry_count = 0;
        }
      } else if (wifi_enabled && retry_count < 5) {
        retry_count++;
        ESP_LOGI(WIFI_TAG, "Retrying connection (attempt %d)...", retry_count);
        esp_wifi_connect();
      } else {
        retry_count = 0;
      }
      break;
    }
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    snprintf(wifi_ip, sizeof(wifi_ip), IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(WIFI_TAG, "Connected! Got IP: %s", wifi_ip);
    wifi_connected = true;

    // Save successful credentials to NVS for auto-reconnect
    if (strlen(wifi_selected_ssid) > 0 && strlen(wifi_password_input) > 0) {
      wifi_save_credentials(wifi_selected_ssid, wifi_password_input);
    }

    // Update UI to show success
    if (lvgl_port_lock(10)) {
      if (wifi_status_label) {
        char status_msg[64];
        snprintf(status_msg, sizeof(status_msg), "Connecte! IP: %s", wifi_ip);
        lv_label_set_text(wifi_status_label, status_msg);
      }
      if (icon_wifi) {
        lv_obj_set_style_text_color(icon_wifi, COLOR_SUCCESS, 0);
      }
      lvgl_port_unlock();
    }
  }
}

// ====================================================================================
// WIFI NVS STORAGE - Save/Load favorite networks
// ====================================================================================

#define NVS_WIFI_NAMESPACE "wifi_creds"
#define NVS_WIFI_SSID_KEY "saved_ssid"
#define NVS_WIFI_PASS_KEY "saved_pass"

// Save WiFi credentials to NVS
static esp_err_t wifi_save_credentials(const char *ssid, const char *password) {
  nvs_handle_t nvs_handle;
  esp_err_t ret = nvs_open(NVS_WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle);
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "Failed to open NVS: %s", esp_err_to_name(ret));
    return ret;
  }

  ret = nvs_set_str(nvs_handle, NVS_WIFI_SSID_KEY, ssid);
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "Failed to save SSID: %s", esp_err_to_name(ret));
    nvs_close(nvs_handle);
    return ret;
  }

  ret = nvs_set_str(nvs_handle, NVS_WIFI_PASS_KEY, password);
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "Failed to save password: %s", esp_err_to_name(ret));
    nvs_close(nvs_handle);
    return ret;
  }

  ret = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  if (ret == ESP_OK) {
    ESP_LOGI(WIFI_TAG, "WiFi credentials saved for SSID: %s", ssid);
  }
  return ret;
}

// Load WiFi credentials from NVS
static esp_err_t wifi_load_credentials(char *ssid, size_t ssid_len,
                                       char *password, size_t pass_len) {
  nvs_handle_t nvs_handle;
  esp_err_t ret = nvs_open(NVS_WIFI_NAMESPACE, NVS_READONLY, &nvs_handle);
  if (ret != ESP_OK) {
    ESP_LOGW(WIFI_TAG, "No saved WiFi credentials found");
    return ret;
  }

  ret = nvs_get_str(nvs_handle, NVS_WIFI_SSID_KEY, ssid, &ssid_len);
  if (ret != ESP_OK) {
    nvs_close(nvs_handle);
    return ret;
  }

  ret = nvs_get_str(nvs_handle, NVS_WIFI_PASS_KEY, password, &pass_len);
  nvs_close(nvs_handle);

  if (ret == ESP_OK) {
    ESP_LOGI(WIFI_TAG, "Loaded saved WiFi credentials for SSID: %s", ssid);
  }
  return ret;
}

// Delete saved WiFi credentials
static esp_err_t wifi_delete_credentials(void) {
  nvs_handle_t nvs_handle;
  esp_err_t ret = nvs_open(NVS_WIFI_NAMESPACE, NVS_READWRITE, &nvs_handle);
  if (ret != ESP_OK) {
    return ret;
  }

  nvs_erase_key(nvs_handle, NVS_WIFI_SSID_KEY);
  nvs_erase_key(nvs_handle, NVS_WIFI_PASS_KEY);
  ret = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  ESP_LOGI(WIFI_TAG, "Saved WiFi credentials deleted");
  return ret;
}

// Check if WiFi credentials are saved
static bool wifi_has_saved_credentials(void) {
  nvs_handle_t nvs_handle;
  if (nvs_open(NVS_WIFI_NAMESPACE, NVS_READONLY, &nvs_handle) != ESP_OK) {
    return false;
  }

  size_t required_size = 0;
  esp_err_t ret =
      nvs_get_str(nvs_handle, NVS_WIFI_SSID_KEY, NULL, &required_size);
  nvs_close(nvs_handle);

  return (ret == ESP_OK && required_size > 1);
}

static esp_err_t wifi_init(void) {
  ESP_LOGI(WIFI_TAG, "Initializing WiFi via ESP32-C6...");

  // Initialize TCP/IP stack
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Create default WiFi station
  sta_netif = esp_netif_create_default_wifi_sta();

  // Initialize WiFi with default config
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Register event handlers
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &wifi_event_handler, NULL));

  // Configure WiFi
  wifi_config_t wifi_cfg = {
      .sta =
          {
              .ssid = WIFI_SSID_DEFAULT,
              .password = WIFI_PASS_DEFAULT,
              .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

  ESP_LOGI(WIFI_TAG, "WiFi initialized, ready to connect");
  return ESP_OK;
}

static esp_err_t wifi_start(void) {
  if (!wifi_enabled) {
    ESP_LOGI(WIFI_TAG, "Starting WiFi...");
    esp_err_t ret = esp_wifi_start();
    if (ret == ESP_OK) {
      wifi_enabled = true;
    }
    return ret;
  }
  return ESP_OK;
}

static esp_err_t wifi_stop(void) {
  if (wifi_enabled) {
    ESP_LOGI(WIFI_TAG, "Stopping WiFi...");
    esp_wifi_disconnect();
    esp_wifi_stop();
    wifi_enabled = false;
    wifi_connected = false;
  }
  return ESP_OK;
}

// WiFi Scan function
static esp_err_t wifi_scan(void) {
  ESP_LOGI(WIFI_TAG, "Starting WiFi scan...");

  // Ensure WiFi is started
  if (!wifi_enabled) {
    ESP_LOGI(WIFI_TAG, "WiFi not enabled, starting...");
    esp_wifi_start();
    wifi_enabled = true;
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for WiFi to fully start
  }

  // Disconnect first to stop connection retry loop - this is required for scan
  // to work
  ESP_LOGI(WIFI_TAG, "Disconnecting to allow scan...");
  esp_wifi_disconnect();
  vTaskDelay(pdMS_TO_TICKS(500)); // Wait for disconnect to complete

  // Stop any ongoing scan first
  esp_wifi_scan_stop();
  vTaskDelay(pdMS_TO_TICKS(100));

  wifi_scan_config_t scan_config = {
      .ssid = NULL,
      .bssid = NULL,
      .channel = 0,
      .show_hidden = true, // Show all networks
      .scan_type = WIFI_SCAN_TYPE_ACTIVE,
      .scan_time.active.min = 120,
      .scan_time.active.max = 300,
  };

  ESP_LOGI(WIFI_TAG, "Starting scan...");
  esp_err_t ret = esp_wifi_scan_start(&scan_config, true); // Blocking scan
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "WiFi scan failed: %s", esp_err_to_name(ret));
    return ret;
  }

  uint16_t ap_count = 0;
  ret = esp_wifi_scan_get_ap_num(&ap_count);
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "Failed to get AP count: %s", esp_err_to_name(ret));
    return ret;
  }

  ESP_LOGI(WIFI_TAG, "Scan found %d APs", ap_count);

  if (ap_count == 0) {
    wifi_scan_count = 0;
    return ESP_OK;
  }

  // Limit to max we can store
  if (ap_count > WIFI_SCAN_MAX_AP) {
    ap_count = WIFI_SCAN_MAX_AP;
  }

  wifi_ap_record_t temp_results[WIFI_SCAN_MAX_AP];
  ret = esp_wifi_scan_get_ap_records(&ap_count, temp_results);
  if (ret != ESP_OK) {
    ESP_LOGE(WIFI_TAG, "Failed to get scan results: %s", esp_err_to_name(ret));
    return ret;
  }

  // Filter out empty SSIDs and copy valid ones
  wifi_scan_count = 0;
  for (int i = 0; i < ap_count && wifi_scan_count < WIFI_SCAN_MAX_AP; i++) {
    // Skip networks with empty SSID
    if (temp_results[i].ssid[0] != '\0') {
      memcpy(&wifi_scan_results[wifi_scan_count], &temp_results[i],
             sizeof(wifi_ap_record_t));
      wifi_scan_count++;
    }
  }

  ESP_LOGI(WIFI_TAG, "Found %d valid networks (filtered from %d)",
           wifi_scan_count, ap_count);
  for (int i = 0; i < wifi_scan_count; i++) {
    ESP_LOGI(WIFI_TAG, "  %d: %s (RSSI: %d)", i + 1, wifi_scan_results[i].ssid,
             wifi_scan_results[i].rssi);
  }

  return ESP_OK;
}

// Connect to specific WiFi
static esp_err_t wifi_connect_to(const char *ssid, const char *password) {
  ESP_LOGI(WIFI_TAG, "Connecting to: %s", ssid);

  wifi_config_t wifi_cfg = {0};
  // Use memcpy with length check to avoid strncpy truncation warnings
  size_t ssid_len = strlen(ssid);
  if (ssid_len > sizeof(wifi_cfg.sta.ssid) - 1) {
    ssid_len = sizeof(wifi_cfg.sta.ssid) - 1;
  }
  memcpy(wifi_cfg.sta.ssid, ssid, ssid_len);

  size_t pass_len = strlen(password);
  if (pass_len > sizeof(wifi_cfg.sta.password) - 1) {
    pass_len = sizeof(wifi_cfg.sta.password) - 1;
  }
  memcpy(wifi_cfg.sta.password, password, pass_len);
  wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

  esp_wifi_disconnect();
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

  esp_err_t ret = esp_wifi_connect();
  if (ret == ESP_OK) {
    snprintf(wifi_ssid, sizeof(wifi_ssid), "%s", ssid);
  }
  return ret;
}

// ====================================================================================
// BLUETOOTH FUNCTIONS (via ESP32-C6 co-processor)
// ====================================================================================

#if CONFIG_BT_ENABLED

static const char *BT_TAG = "BLUETOOTH";

// Bluetooth state
static bool bt_initialized = false;
static bool bt_scanning = false;
static bool bt_connecting = false;
static bool bt_scan_update_pending = false; // Flag to update UI from main loop
static int bt_selected_device_idx = -1;

// BLE scan results storage
#define BT_SCAN_MAX_DEVICES 10
#define BLE_DEVICE_NAME_MAX_LEN 32 // Max BLE device name length
typedef struct {
  esp_bd_addr_t bda;
  char name[BLE_DEVICE_NAME_MAX_LEN + 1];
  int rssi;
  bool valid;
} bt_device_info_t;

static bt_device_info_t bt_scan_results[BT_SCAN_MAX_DEVICES];
static int bt_scan_count = 0;

// Helper function to convert BDA to string
static char *bda_to_str(esp_bd_addr_t bda, char *str, size_t size) {
  if (bda == NULL || str == NULL || size < 18) {
    return NULL;
  }
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", bda[0], bda[1], bda[2], bda[3],
          bda[4], bda[5]);
  return str;
}

// BLE GAP event callback
static void bt_gap_ble_cb(esp_gap_ble_cb_event_t event,
                          esp_ble_gap_cb_param_t *param) {
  char bda_str[18];

  switch (event) {
  case ESP_GAP_BLE_SCAN_RESULT_EVT:
    if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
      // Found a BLE device
      ESP_LOGI(BT_TAG, "BLE Device found: %s, RSSI: %d",
               bda_to_str(param->scan_rst.bda, bda_str, sizeof(bda_str)),
               param->scan_rst.rssi);

      // Store device info if we have space - check for duplicates first
      int existing_idx = -1;
      for (int i = 0; i < bt_scan_count; i++) {
        if (memcmp(bt_scan_results[i].bda, param->scan_rst.bda,
                   sizeof(esp_bd_addr_t)) == 0) {
          existing_idx = i;
          break;
        }
      }

      // Use existing slot or new slot if not a duplicate
      int slot_idx = (existing_idx >= 0) ? existing_idx : bt_scan_count;

      if (slot_idx < BT_SCAN_MAX_DEVICES) {
        memcpy(bt_scan_results[slot_idx].bda, param->scan_rst.bda,
               sizeof(esp_bd_addr_t));
        bt_scan_results[slot_idx].rssi = param->scan_rst.rssi;

        // Try to get device name from advertising data
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
        adv_name = esp_ble_resolve_adv_data(
            param->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
        if (adv_name == NULL) {
          adv_name = esp_ble_resolve_adv_data(param->scan_rst.ble_adv,
                                              ESP_BLE_AD_TYPE_NAME_SHORT,
                                              &adv_name_len);
        }

        if (adv_name && adv_name_len > 0) {
          int copy_len = (adv_name_len > BLE_DEVICE_NAME_MAX_LEN)
                             ? BLE_DEVICE_NAME_MAX_LEN
                             : adv_name_len;
          memcpy(bt_scan_results[slot_idx].name, adv_name, copy_len);
          bt_scan_results[slot_idx].name[copy_len] = '\0';
          if (existing_idx < 0) {
            ESP_LOGI(BT_TAG, "  Name: %s", bt_scan_results[slot_idx].name);
          }
        } else if (existing_idx < 0 ||
                   strcmp(bt_scan_results[slot_idx].name, "(Unknown)") == 0) {
          // Only set to Unknown if it's new or was already Unknown
          snprintf(bt_scan_results[slot_idx].name,
                   sizeof(bt_scan_results[slot_idx].name), "(Unknown)");
        }

        bt_scan_results[slot_idx].valid = true;

        // Only increment count if this is a new device
        if (existing_idx < 0) {
          bt_scan_count++;
        }
      }
    } else if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
      ESP_LOGI(BT_TAG, "BLE Scan complete, found %d devices", bt_scan_count);
      bt_scanning = false;
      bt_scan_update_pending = true; // Signal UI update needed
    }
    break;

  case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
    if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
      ESP_LOGI(BT_TAG, "BLE scan started successfully");
      bt_scanning = true;
    } else {
      ESP_LOGE(BT_TAG, "BLE scan start failed: %d",
               param->scan_start_cmpl.status);
      bt_scanning = false;
    }
    break;

  case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
    ESP_LOGI(BT_TAG, "BLE scan stopped");
    bt_scanning = false;
    break;

  case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    ESP_LOGI(BT_TAG, "Advertising data set complete");
    break;

  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
      ESP_LOGI(BT_TAG,
               "Advertising started - Device visible as 'Reptile Panel'");
    } else {
      ESP_LOGW(BT_TAG, "Advertising start failed: %d",
               param->adv_start_cmpl.status);
    }
    break;

  case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    ESP_LOGI(BT_TAG, "Advertising stopped");
    break;

  case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    ESP_LOGI(
        BT_TAG,
        "Connection params updated: status=%d, conn_int=%d, latency=%d, "
        "timeout=%d",
        param->update_conn_params.status, param->update_conn_params.conn_int,
        param->update_conn_params.latency, param->update_conn_params.timeout);
    break;

  default:
    ESP_LOGD(BT_TAG, "BLE GAP event: %d", event);
    break;
  }
}

// Initialize Bluetooth via ESP-Hosted
static esp_err_t bluetooth_init(void) {
  ESP_LOGI(BT_TAG, "Initializing Bluetooth via ESP32-C6...");

  // Ensure ESP-Hosted connection is established
  // This should already be done by WiFi, but let's make sure
  esp_err_t ret = esp_hosted_connect_to_slave();
  if (ret != ESP_OK) {
    ESP_LOGW(BT_TAG,
             "esp_hosted_connect_to_slave: %s (may already be connected)",
             esp_err_to_name(ret));
    // Continue anyway, might already be connected
  }

  // Initialize Bluetooth controller on ESP32-C6
  ret = esp_hosted_bt_controller_init();
  if (ret != ESP_OK) {
    ESP_LOGW(BT_TAG, "BT controller init: %s (may already be initialized)",
             esp_err_to_name(ret));
    // Continue anyway, might already be initialized
  }

  // Enable Bluetooth controller
  ret = esp_hosted_bt_controller_enable();
  if (ret != ESP_OK) {
    ESP_LOGW(BT_TAG, "BT controller enable: %s (may already be enabled)",
             esp_err_to_name(ret));
    // Continue anyway, might already be enabled
  }

  // Open HCI transport for Bluedroid
  hosted_hci_bluedroid_open();

  // Get and attach HCI driver operations
  esp_bluedroid_hci_driver_operations_t hci_ops = {
      .send = hosted_hci_bluedroid_send,
      .check_send_available = hosted_hci_bluedroid_check_send_available,
      .register_host_callback = hosted_hci_bluedroid_register_host_callback,
  };
  esp_bluedroid_attach_hci_driver(&hci_ops);

  // Initialize Bluedroid stack
  ret = esp_bluedroid_init();
  if (ret != ESP_OK) {
    ESP_LOGE(BT_TAG, "Failed to init Bluedroid: %s", esp_err_to_name(ret));
    return ret;
  }

  // Enable Bluedroid
  ret = esp_bluedroid_enable();
  if (ret != ESP_OK) {
    ESP_LOGE(BT_TAG, "Failed to enable Bluedroid: %s", esp_err_to_name(ret));
    return ret;
  }

  // Register BLE GAP callback
  ret = esp_ble_gap_register_callback(bt_gap_ble_cb);
  if (ret != ESP_OK) {
    ESP_LOGE(BT_TAG, "Failed to register BLE GAP callback: %s",
             esp_err_to_name(ret));
    return ret;
  }

  // Set device name - "Reptile Panel"
  esp_ble_gap_set_device_name("Reptile Panel");

  // Configure BLE advertising parameters
  esp_ble_adv_params_t adv_params = {
      .adv_int_min = 0x20,      // 20ms minimum interval
      .adv_int_max = 0x40,      // 40ms maximum interval
      .adv_type = ADV_TYPE_IND, // Connectable undirected advertising
      .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .channel_map = ADV_CHNL_ALL,
      .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
  };

  // Configure advertising data
  esp_ble_adv_data_t adv_data = {
      .set_scan_rsp = false,
      .include_name = true,
      .include_txpower = true,
      .min_interval = 0x0006, // 7.5ms
      .max_interval = 0x0010, // 20ms
      .appearance = 0x00,
      .manufacturer_len = 0,
      .p_manufacturer_data = NULL,
      .service_data_len = 0,
      .p_service_data = NULL,
      .service_uuid_len = 0,
      .p_service_uuid = NULL,
      .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
  };

  // Set advertising data
  ret = esp_ble_gap_config_adv_data(&adv_data);
  if (ret != ESP_OK) {
    ESP_LOGW(BT_TAG, "Failed to config adv data: %s", esp_err_to_name(ret));
  }

  // Start advertising (will be visible to other devices)
  ret = esp_ble_gap_start_advertising(&adv_params);
  if (ret != ESP_OK) {
    ESP_LOGW(BT_TAG, "Failed to start advertising: %s", esp_err_to_name(ret));
  } else {
    ESP_LOGI(BT_TAG, "BLE Advertising started - Device name: 'Reptile Panel'");
  }

  bt_initialized = true;
  ESP_LOGI(BT_TAG, "Bluetooth initialized successfully");
  return ESP_OK;
}

// Start BLE scan
static esp_err_t bluetooth_start_scan(uint32_t duration_sec) {
  if (!bt_initialized) {
    ESP_LOGW(BT_TAG, "Bluetooth not initialized");
    return ESP_ERR_INVALID_STATE;
  }

  // Always try to stop first if scanning
  if (bt_scanning) {
    ESP_LOGI(BT_TAG, "Stopping ongoing scan before restart...");
    esp_ble_gap_stop_scanning();
    bt_scanning = false;
    vTaskDelay(pdMS_TO_TICKS(200)); // Wait for scan to fully stop
  }

  // Clear previous scan results
  memset(bt_scan_results, 0, sizeof(bt_scan_results));
  bt_scan_count = 0;

  // Configure scan parameters
  esp_ble_scan_params_t scan_params = {
      .scan_type = BLE_SCAN_TYPE_ACTIVE,
      .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
      .scan_interval = 0x50, // 50ms
      .scan_window = 0x30,   // 30ms
      .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
  };

  esp_err_t ret = esp_ble_gap_set_scan_params(&scan_params);
  if (ret != ESP_OK) {
    ESP_LOGE(BT_TAG, "Failed to set scan params: %s", esp_err_to_name(ret));
    return ret;
  }

  // Start scanning
  ret = esp_ble_gap_start_scanning(duration_sec);
  if (ret != ESP_OK) {
    ESP_LOGE(BT_TAG, "Failed to start scan: %s", esp_err_to_name(ret));
    return ret;
  }

  ESP_LOGI(BT_TAG, "BLE scan started for %lu seconds", duration_sec);
  return ESP_OK;
}

// Stop BLE scan
static esp_err_t bluetooth_stop_scan(void) {
  if (!bt_initialized || !bt_scanning) {
    return ESP_OK;
  }

  return esp_ble_gap_stop_scanning();
}

#else // CONFIG_BT_ENABLED not defined

// Stub function when Bluetooth is disabled
static esp_err_t bluetooth_init(void) {
  ESP_LOGW("BLUETOOTH", "Bluetooth disabled in sdkconfig - skipping init");
  return ESP_ERR_NOT_SUPPORTED;
}

#endif // CONFIG_BT_ENABLED

// ====================================================================================
// ESP32-C6 CO-PROCESSOR OTA UPDATE
// ====================================================================================

static const char *OTA_TAG = "C6_OTA";
static bool ota_in_progress = false;
static int ota_progress_percent = 0;
static lv_obj_t *ota_progress_label = NULL;
static lv_obj_t *ota_progress_bar = NULL;

// Perform OTA update of ESP32-C6 co-processor
static esp_err_t perform_c6_ota_update(void) {
  if (ota_in_progress) {
    ESP_LOGW(OTA_TAG, "OTA already in progress");
    return ESP_ERR_INVALID_STATE;
  }

  size_t fw_size = slave_fw_end - slave_fw_start;
  ESP_LOGI(OTA_TAG, "Starting ESP32-C6 OTA update, firmware size: %zu bytes",
           fw_size);

  if (fw_size == 0) {
    ESP_LOGE(OTA_TAG, "Firmware binary is empty!");
    return ESP_ERR_INVALID_SIZE;
  }

  ota_in_progress = true;
  ota_progress_percent = 0;

  // Step 1: Begin OTA
  ESP_LOGI(OTA_TAG, "Step 1/4: Beginning OTA...");
  esp_err_t ret = esp_hosted_slave_ota_begin();
  if (ret != ESP_OK) {
    ESP_LOGE(OTA_TAG, "Failed to begin OTA: %s", esp_err_to_name(ret));
    ota_in_progress = false;
    return ret;
  }

  // Step 2: Write firmware in chunks
  ESP_LOGI(OTA_TAG, "Step 2/4: Writing firmware...");
  const size_t chunk_size = 1400; // Optimal chunk size for transport
  size_t written = 0;
  uint8_t *ptr = (uint8_t *)slave_fw_start; // Cast away const for API

  while (written < fw_size) {
    size_t to_write =
        (fw_size - written) > chunk_size ? chunk_size : (fw_size - written);

    ret = esp_hosted_slave_ota_write(ptr, to_write);
    if (ret != ESP_OK) {
      ESP_LOGE(OTA_TAG, "Failed to write OTA data at offset %zu: %s", written,
               esp_err_to_name(ret));
      ota_in_progress = false;
      return ret;
    }

    written += to_write;
    ptr += to_write;
    ota_progress_percent = (written * 100) / fw_size;

    // Log progress every 10%
    if ((written * 10 / fw_size) != ((written - to_write) * 10 / fw_size)) {
      ESP_LOGI(OTA_TAG, "OTA Progress: %d%% (%zu/%zu bytes)",
               ota_progress_percent, written, fw_size);
    }

    // Yield to prevent watchdog
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  // Step 3: End OTA (validate)
  ESP_LOGI(OTA_TAG, "Step 3/4: Validating firmware...");
  ret = esp_hosted_slave_ota_end();
  if (ret != ESP_OK) {
    ESP_LOGE(OTA_TAG, "Failed to end OTA: %s", esp_err_to_name(ret));
    ota_in_progress = false;
    return ret;
  }

  // Step 4: Activate new firmware
  ESP_LOGI(OTA_TAG, "Step 4/4: Activating new firmware...");
  ret = esp_hosted_slave_ota_activate();
  if (ret != ESP_OK) {
    ESP_LOGE(OTA_TAG, "Failed to activate OTA: %s", esp_err_to_name(ret));
    ota_in_progress = false;
    return ret;
  }

  ESP_LOGI(OTA_TAG,
           "ESP32-C6 OTA update completed successfully! Rebooting C6...");
  ota_in_progress = false;
  ota_progress_percent = 100;

  return ESP_OK;
}

// ====================================================================================
// FRENCH AZERTY KEYBOARD LAYOUT - Based on LVGL official example
// ====================================================================================

// Lowercase AZERTY layout
static const char *kb_map_azerty_lower[] = {"1",
                                            "2",
                                            "3",
                                            "4",
                                            "5",
                                            "6",
                                            "7",
                                            "8",
                                            "9",
                                            "0",
                                            LV_SYMBOL_BACKSPACE,
                                            "\n",
                                            "a",
                                            "z",
                                            "e",
                                            "r",
                                            "t",
                                            "y",
                                            "u",
                                            "i",
                                            "o",
                                            "p",
                                            "\n",
                                            "q",
                                            "s",
                                            "d",
                                            "f",
                                            "g",
                                            "h",
                                            "j",
                                            "k",
                                            "l",
                                            "m",
                                            LV_SYMBOL_NEW_LINE,
                                            "\n",
                                            "ABC",
                                            "w",
                                            "x",
                                            "c",
                                            "v",
                                            "b",
                                            "n",
                                            ",",
                                            ".",
                                            "?",
                                            "\n",
                                            "1#",
                                            LV_SYMBOL_LEFT,
                                            " ",
                                            " ",
                                            " ",
                                            LV_SYMBOL_RIGHT,
                                            LV_SYMBOL_OK,
                                            ""};

// Uppercase AZERTY layout
static const char *kb_map_azerty_upper[] = {"!",
                                            "@",
                                            "#",
                                            "$",
                                            "%",
                                            "^",
                                            "&",
                                            "*",
                                            "(",
                                            ")",
                                            LV_SYMBOL_BACKSPACE,
                                            "\n",
                                            "A",
                                            "Z",
                                            "E",
                                            "R",
                                            "T",
                                            "Y",
                                            "U",
                                            "I",
                                            "O",
                                            "P",
                                            "\n",
                                            "Q",
                                            "S",
                                            "D",
                                            "F",
                                            "G",
                                            "H",
                                            "J",
                                            "K",
                                            "L",
                                            "M",
                                            LV_SYMBOL_NEW_LINE,
                                            "\n",
                                            "abc",
                                            "W",
                                            "X",
                                            "C",
                                            "V",
                                            "B",
                                            "N",
                                            ";",
                                            ":",
                                            "!",
                                            "\n",
                                            "1#",
                                            LV_SYMBOL_LEFT,
                                            " ",
                                            " ",
                                            " ",
                                            LV_SYMBOL_RIGHT,
                                            LV_SYMBOL_OK,
                                            ""};

// Special characters layout
static const char *kb_map_special[] = {"1",
                                       "2",
                                       "3",
                                       "4",
                                       "5",
                                       "6",
                                       "7",
                                       "8",
                                       "9",
                                       "0",
                                       LV_SYMBOL_BACKSPACE,
                                       "\n",
                                       "+",
                                       "-",
                                       "*",
                                       "/",
                                       "=",
                                       "_",
                                       "<",
                                       ">",
                                       "[",
                                       "]",
                                       "\n",
                                       "{",
                                       "}",
                                       "|",
                                       "\\",
                                       "~",
                                       "`",
                                       "'",
                                       "\"",
                                       ":",
                                       ";",
                                       LV_SYMBOL_NEW_LINE,
                                       "\n",
                                       "abc",
                                       "@",
                                       "#",
                                       "$",
                                       "%",
                                       "^",
                                       "&",
                                       ",",
                                       ".",
                                       "?",
                                       "\n",
                                       "ABC",
                                       LV_SYMBOL_LEFT,
                                       " ",
                                       " ",
                                       " ",
                                       LV_SYMBOL_RIGHT,
                                       LV_SYMBOL_OK,
                                       ""};

// Control map for keyboard buttons (defines button widths and special flags)
// LVGL 9 uses LV_KEYBOARD_CTRL_BTN_FLAGS for buttons that should change mode
// Row 1: 11 buttons (numbers + backspace)
// Row 2: 10 buttons (letters a-p)
// Row 3: 11 buttons (letters q-m + enter)
// Row 4: 10 buttons (shift + letters + punctuation)
// Row 5: 7 buttons (123 + arrows + spaces + OK)

// Flag to mark mode-switching buttons
#define KB_CTRL_MODE_BTN                                                       \
  (LV_BUTTONMATRIX_CTRL_CHECKED | LV_BUTTONMATRIX_CTRL_NO_REPEAT |             \
   LV_BUTTONMATRIX_CTRL_CLICK_TRIG)

static const lv_buttonmatrix_ctrl_t kb_ctrl_lower[] = {
    // Row 1: numbers + backspace (wider)
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 2: letters
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 3: letters + enter (wider)
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 4: ABC (mode switch) + letters + punctuation
    6 | KB_CTRL_MODE_BTN, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 5: 123 (mode switch) + arrows + spaces + OK
    5 | KB_CTRL_MODE_BTN, 3, 7, 7, 7, 3, 5 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG};

static const lv_buttonmatrix_ctrl_t kb_ctrl_upper[] = {
    // Row 1: numbers + backspace
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 2: letters
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 3: letters + enter
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 4: abc (mode switch) + letters
    6 | KB_CTRL_MODE_BTN, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 5: 123 + arrows + spaces + OK
    5 | KB_CTRL_MODE_BTN, 3, 7, 7, 7, 3, 5 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG};

static const lv_buttonmatrix_ctrl_t kb_ctrl_special[] = {
    // Row 1: numbers + backspace
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 2: special chars
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 3: special chars + enter
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG,
    // Row 4: abc (mode switch) + special chars
    6 | KB_CTRL_MODE_BTN, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    // Row 5: ABC + arrows + spaces + OK
    5 | KB_CTRL_MODE_BTN, 3, 7, 7, 7, 3, 5 | LV_BUTTONMATRIX_CTRL_CLICK_TRIG};

// ====================================================================================
// HARDWARE FUNCTIONS
// ====================================================================================

static esp_err_t enable_dsi_phy_power(void) {
  static esp_ldo_channel_handle_t phy_pwr_chan = NULL;
  if (phy_pwr_chan)
    return ESP_OK;
  esp_ldo_channel_config_t ldo_cfg = {
      .chan_id = DSI_PHY_LDO_CHANNEL,
      .voltage_mv = DSI_PHY_VOLTAGE_MV,
  };
  return esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan);
}

static esp_err_t backlight_init(void) {
  ledc_timer_config_t timer_cfg = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = LEDC_TIMER_10_BIT,
      .timer_num = BL_LEDC_TIMER,
      .freq_hz = BL_PWM_FREQ,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

  ledc_channel_config_t ch_cfg = {
      .gpio_num = LCD_BL_GPIO,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = BL_LEDC_CHANNEL,
      .timer_sel = BL_LEDC_TIMER,
      .duty = 0,
      .hpoint = 0,
  };
  return ledc_channel_config(&ch_cfg);
}

static void backlight_set(uint8_t percent) {
  if (percent > 100)
    percent = 100;
  current_brightness = percent;
  uint32_t duty = (percent * 1023) / 100;
  ledc_set_duty(LEDC_LOW_SPEED_MODE, BL_LEDC_CHANNEL, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, BL_LEDC_CHANNEL);
}

static esp_err_t i2c_init(void) {
  i2c_master_bus_config_t bus_config = {
      .i2c_port = I2C_NUM_0,
      .sda_io_num = TOUCH_I2C_SDA,
      .scl_io_num = TOUCH_I2C_SCL,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = false,
  };
  return i2c_new_master_bus(&bus_config, &i2c_bus_handle);
}

static esp_err_t touch_init(void) {
  if (!i2c_bus_handle)
    ESP_ERROR_CHECK(i2c_init());

  esp_lcd_panel_io_handle_t touch_io = NULL;
  esp_lcd_panel_io_i2c_config_t io_config = {
      .dev_addr = 0x5D,
      .scl_speed_hz = TOUCH_I2C_FREQ_HZ,
      .control_phase_bytes = 1,
      .lcd_cmd_bits = 16,
      .lcd_param_bits = 0,
      .dc_bit_offset = 0,
      .flags = {.disable_control_phase = 1},
  };
  ESP_ERROR_CHECK(
      esp_lcd_new_panel_io_i2c(i2c_bus_handle, &io_config, &touch_io));

  esp_lcd_touch_config_t touch_cfg = {
      .x_max = LCD_H_RES,
      .y_max = LCD_V_RES,
      .rst_gpio_num = GPIO_NUM_NC,
      .int_gpio_num = GPIO_NUM_NC,
      .levels = {.reset = 0, .interrupt = 0},
      .flags = {.swap_xy = 0, .mirror_x = 0, .mirror_y = 0},
  };
  return esp_lcd_touch_new_i2c_gt911(touch_io, &touch_cfg, &touch_handle);
}

static esp_err_t display_init(esp_lcd_panel_io_handle_t *out_io,
                              esp_lcd_panel_handle_t *out_panel) {
  ESP_ERROR_CHECK(enable_dsi_phy_power());
  vTaskDelay(pdMS_TO_TICKS(10));

  esp_lcd_dsi_bus_handle_t dsi_bus;
  esp_lcd_dsi_bus_config_t bus_cfg = {
      .bus_id = 0,
      .num_data_lanes = DSI_LANE_NUM,
      .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
      .lane_bit_rate_mbps = DSI_LANE_BITRATE,
  };
  ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_cfg, &dsi_bus));
  vTaskDelay(pdMS_TO_TICKS(50));

  esp_lcd_panel_io_handle_t panel_io;
  esp_lcd_dbi_io_config_t dbi_cfg = {
      .virtual_channel = 0, .lcd_cmd_bits = 8, .lcd_param_bits = 8};
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(dsi_bus, &dbi_cfg, &panel_io));

  // DPI configuration for JD9165BA 7-inch 1024x600 panel
  // Timings from datasheet: VS=2, VBP=21, VFP=12, HS=24, HBP=136, HFP=160
  esp_lcd_dpi_panel_config_t dpi_cfg = {
      .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
      .dpi_clock_freq_mhz = DPI_CLOCK_MHZ, // 52 MHz (~51.2 MHz from datasheet)
      .virtual_channel = 0,
      .in_color_format = LCD_COLOR_FMT_RGB565,
      .num_fbs = 1,
      .video_timing =
          {
              .h_size = LCD_H_RES,      // 1024
              .v_size = LCD_V_RES,      // 600
              .hsync_pulse_width = 24,  // HS = 24
              .hsync_back_porch = 136,  // HBP = 136
              .hsync_front_porch = 160, // HFP = 160
              .vsync_pulse_width = 2,   // VS = 2
              .vsync_back_porch = 21,   // VBP = 21
              .vsync_front_porch = 12,  // VFP = 12
          },
  };

  // Using ST7701 driver with JD9165BA init commands (generic MIPI-DCS)
  st7701_vendor_config_t vendor_cfg = {
      .flags.use_mipi_interface = 1,
      .mipi_config = {.dsi_bus = dsi_bus, .dpi_config = &dpi_cfg},
      .init_cmds = jd9165ba_lcd_cmds, // Use JD9165BA commands
      .init_cmds_size =
          sizeof(jd9165ba_lcd_cmds) / sizeof(jd9165ba_lcd_cmds[0]),
  };

  esp_lcd_panel_dev_config_t panel_cfg = {
      .reset_gpio_num = LCD_RST_GPIO,
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
      .bits_per_pixel = 16,
      .vendor_config = &vendor_cfg,
  };

  esp_lcd_panel_handle_t panel;
  ESP_ERROR_CHECK(esp_lcd_new_panel_st7701(panel_io, &panel_cfg, &panel));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
  vTaskDelay(pdMS_TO_TICKS(50));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
  vTaskDelay(pdMS_TO_TICKS(120)); // JD9165BA needs 120ms after init
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

  *out_io = panel_io;
  *out_panel = panel;
  ESP_LOGI(TAG, "7-inch JD9165BA display initialized (1024x600)");
  return ESP_OK;
}

// ====================================================================================
// UI HELPER FUNCTIONS
// ====================================================================================

static lv_obj_t *create_card(lv_obj_t *parent, int w, int h) {
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_size(card, w, h);
  lv_obj_set_style_bg_color(card, COLOR_BG_CARD, 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, 16, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, COLOR_BORDER, 0);
  lv_obj_set_style_pad_all(card, 16, 0);
  lv_obj_set_style_shadow_width(card, 0, 0);
  return card;
}

static void show_page(lv_obj_t *page) {
  if (page_home)
    lv_obj_add_flag(page_home, LV_OBJ_FLAG_HIDDEN);
  if (page_settings)
    lv_obj_add_flag(page_settings, LV_OBJ_FLAG_HIDDEN);
  if (page_wifi)
    lv_obj_add_flag(page_wifi, LV_OBJ_FLAG_HIDDEN);
  if (page_bluetooth)
    lv_obj_add_flag(page_bluetooth, LV_OBJ_FLAG_HIDDEN);
  if (page)
    lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
}

// ====================================================================================
// EVENT CALLBACKS
// ====================================================================================

static void nav_home_cb(lv_event_t *e) {
  (void)e;
  show_page(page_home);
}
static void nav_settings_cb(lv_event_t *e) {
  (void)e;
  show_page(page_settings);
}

static void brightness_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  backlight_set((uint8_t)lv_slider_get_value(slider));
}

static void wifi_toggle_cb(lv_event_t *e) {
  lv_obj_t *sw = lv_event_get_target(e);
  bool enable = lv_obj_has_state(sw, LV_STATE_CHECKED);

  if (enable) {
    wifi_start();
  } else {
    wifi_stop();
  }

  if (icon_wifi) {
    lv_obj_set_style_text_color(
        icon_wifi,
        wifi_enabled ? (wifi_connected ? COLOR_SUCCESS : COLOR_WARNING)
                     : COLOR_TEXT_DIM,
        0);
  }
  ESP_LOGI(TAG, "WiFi %s", wifi_enabled ? "enabled" : "disabled");
}

static void bluetooth_toggle_cb(lv_event_t *e) {
  lv_obj_t *sw = lv_event_get_target(e);
  bool enable = lv_obj_has_state(sw, LV_STATE_CHECKED);
  bluetooth_enabled = enable;

  if (enable) {
#if CONFIG_BT_ENABLED
    // Initialize Bluetooth if not already done
    if (!bt_initialized) {
      esp_err_t ret = bluetooth_init();
      if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Bluetooth: %s",
                 esp_err_to_name(ret));
      }
    }
#endif
    // Navigate to Bluetooth configuration page
    show_page(page_bluetooth);
  }

  if (icon_bluetooth) {
    lv_obj_set_style_text_color(
        icon_bluetooth, bluetooth_enabled ? COLOR_PRIMARY : COLOR_TEXT_DIM, 0);
  }
  ESP_LOGI(TAG, "Bluetooth %s", bluetooth_enabled ? "enabled" : "disabled");
}

// WiFi Page Callbacks
static void wifi_scan_btn_cb(lv_event_t *e);
static void wifi_list_cb(lv_event_t *e);
static void wifi_connect_btn_cb(lv_event_t *e);
static void wifi_back_btn_cb(lv_event_t *e);
static void wifi_keyboard_ready_cb(lv_event_t *e);
// Note: Mode switching (ABC/abc/1#) is handled automatically by LVGL's default
// keyboard handler

static void update_wifi_list(void) {
  if (!wifi_list)
    return;
  lv_obj_clean(wifi_list);

  // Limit displayed networks to avoid memory issues
  int display_count = wifi_scan_count > 10 ? 10 : wifi_scan_count;

  for (int i = 0; i < display_count; i++) {
    // Skip if SSID is empty
    if (wifi_scan_results[i].ssid[0] == '\0')
      continue;

    lv_obj_t *btn = lv_list_add_button(wifi_list, LV_SYMBOL_WIFI,
                                       (const char *)wifi_scan_results[i].ssid);
    lv_obj_add_event_cb(btn, wifi_list_cb, LV_EVENT_CLICKED,
                        (void *)(intptr_t)i);
    lv_obj_set_style_bg_color(btn, COLOR_BG_CARD, 0);
    lv_obj_set_style_text_color(btn, COLOR_TEXT, 0);

    // Add signal strength indicator
    char rssi_str[16];
    snprintf(rssi_str, sizeof(rssi_str), "%d dBm", wifi_scan_results[i].rssi);
    lv_obj_t *rssi_label = lv_label_create(btn);
    lv_label_set_text(rssi_label, rssi_str);
    lv_obj_set_style_text_color(rssi_label, COLOR_TEXT_DIM, 0);
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_12, 0);
    lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, -10, 0);
  }
}

static void wifi_scan_btn_cb(lv_event_t *e) {
  (void)e;
  if (wifi_status_label) {
    lv_label_set_text(wifi_status_label, "Scanning...");
  }

  // Force redraw before blocking scan
  lv_refr_now(NULL);

  // Release LVGL lock during blocking scan
  lvgl_port_unlock();

  // Perform blocking scan
  wifi_scan();

  // Brief delay to let other tasks run (avoid watchdog)
  vTaskDelay(pdMS_TO_TICKS(10));

  // Reacquire LVGL lock - wait indefinitely, we need it
  lvgl_port_lock(portMAX_DELAY);

  // Update the list
  if (wifi_list) {
    lv_obj_clean(wifi_list);

    // Limit displayed networks to avoid memory issues
    int display_count = wifi_scan_count > 10 ? 10 : wifi_scan_count;

    for (int i = 0; i < display_count; i++) {
      // Skip if SSID is empty (extra safety)
      if (wifi_scan_results[i].ssid[0] == '\0')
        continue;

      lv_obj_t *btn = lv_list_add_button(
          wifi_list, LV_SYMBOL_WIFI, (const char *)wifi_scan_results[i].ssid);
      lv_obj_add_event_cb(btn, wifi_list_cb, LV_EVENT_CLICKED,
                          (void *)(intptr_t)i);
      lv_obj_set_style_bg_color(btn, COLOR_BG_CARD, 0);
      lv_obj_set_style_text_color(btn, COLOR_TEXT, 0);

      // Add signal strength indicator
      char rssi_str[16];
      snprintf(rssi_str, sizeof(rssi_str), "%d dBm", wifi_scan_results[i].rssi);
      lv_obj_t *rssi_label = lv_label_create(btn);
      lv_label_set_text(rssi_label, rssi_str);
      lv_obj_set_style_text_color(rssi_label, COLOR_TEXT_DIM, 0);
      lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_12, 0);
      lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, -10, 0);

      // Yield to avoid blocking too long
      if (i % 3 == 2) {
        lv_refr_now(NULL);
      }
    }
  }

  if (wifi_status_label) {
    char status[64];
    snprintf(status, sizeof(status), "Found %d networks", wifi_scan_count);
    lv_label_set_text(wifi_status_label, status);
  }

  // Note: We keep the lock - LVGL port event handler expects it
}

static void wifi_list_cb(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
  if (idx >= 0 && idx < wifi_scan_count) {
    snprintf(wifi_selected_ssid, sizeof(wifi_selected_ssid), "%s",
             (const char *)wifi_scan_results[idx].ssid);

    if (wifi_ssid_label) {
      char ssid_text[64];
      snprintf(ssid_text, sizeof(ssid_text), "Network: %s", wifi_selected_ssid);
      lv_label_set_text(wifi_ssid_label, ssid_text);
    }

    // Show password container and keyboard
    if (wifi_pwd_container) {
      lv_obj_clear_flag(wifi_pwd_container, LV_OBJ_FLAG_HIDDEN);
    }
    if (wifi_password_ta) {
      lv_textarea_set_text(wifi_password_ta, "");
      lv_textarea_set_password_mode(wifi_password_ta,
                                    true); // Reset to password mode
    }
    if (wifi_keyboard) {
      lv_obj_clear_flag(wifi_keyboard, LV_OBJ_FLAG_HIDDEN);
    }

    ESP_LOGI(TAG, "Selected network: %s", wifi_selected_ssid);
  }
}

// Custom keyboard ready event - triggered when OK button is pressed
static void wifi_keyboard_ready_cb(lv_event_t *e) {
  lv_obj_t *kb = lv_event_get_target(e);
  (void)kb;

  // Handle OK button - connect to WiFi
  if (wifi_password_ta) {
    const char *pwd = lv_textarea_get_text(wifi_password_ta);
    snprintf(wifi_password_input, sizeof(wifi_password_input), "%s", pwd);
  }
  if (strlen(wifi_selected_ssid) > 0) {
    wifi_connect_to(wifi_selected_ssid, wifi_password_input);

    if (wifi_status_label) {
      lv_label_set_text(wifi_status_label, "Connecting...");
    }
  }

  // Hide keyboard and password container
  if (wifi_keyboard) {
    lv_obj_add_flag(wifi_keyboard, LV_OBJ_FLAG_HIDDEN);
  }
  if (wifi_pwd_container) {
    lv_obj_add_flag(wifi_pwd_container, LV_OBJ_FLAG_HIDDEN);
  }
}

// Note: Mode switching (ABC/abc/1#) is now handled automatically by LVGL's
// default keyboard handler. The button texts "ABC", "abc", "1#" are recognized
// and trigger mode changes without needing a custom handler.

static void wifi_connect_btn_cb(lv_event_t *e) {
  (void)e;
  if (wifi_password_ta) {
    const char *pwd = lv_textarea_get_text(wifi_password_ta);
    snprintf(wifi_password_input, sizeof(wifi_password_input), "%s", pwd);
  }

  if (strlen(wifi_selected_ssid) > 0) {
    wifi_connect_to(wifi_selected_ssid, wifi_password_input);
    if (wifi_status_label) {
      lv_label_set_text(wifi_status_label, "Connecting...");
    }
  }
}

// Toggle password visibility
static void wifi_password_toggle_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  if (wifi_password_ta) {
    bool is_password = lv_textarea_get_password_mode(wifi_password_ta);
    lv_textarea_set_password_mode(wifi_password_ta, !is_password);

    // Update eye icon
    lv_obj_t *lbl = lv_obj_get_child(btn, 0);
    if (lbl) {
      lv_label_set_text(lbl,
                        is_password ? LV_SYMBOL_EYE_OPEN : LV_SYMBOL_EYE_CLOSE);
    }
  }
}

static void wifi_back_btn_cb(lv_event_t *e) {
  (void)e;
  show_page(page_settings);
}

// Forget saved WiFi network
static void wifi_forget_btn_cb(lv_event_t *e) {
  (void)e;
  ESP_LOGI(WIFI_TAG, "Forgetting saved WiFi network...");

  // Disconnect first if connected
  if (wifi_connected) {
    esp_wifi_disconnect();
    wifi_connected = false;
  }

  // Delete saved credentials
  wifi_delete_credentials();

  // Reset UI
  if (lvgl_port_lock(10)) {
    if (wifi_status_label) {
      lv_label_set_text(wifi_status_label,
                        "Reseau oublie. Scannez pour reconnecter.");
    }
    if (wifi_ssid_label) {
      lv_label_set_text(wifi_ssid_label, "Network: (none selected)");
    }
    if (icon_wifi) {
      lv_obj_set_style_text_color(icon_wifi, COLOR_TEXT_DIM, 0);
    }
    lvgl_port_unlock();
  }

  memset(wifi_selected_ssid, 0, sizeof(wifi_selected_ssid));
  memset(wifi_ip, 0, sizeof(wifi_ip));
  ESP_LOGI(WIFI_TAG, "WiFi network forgotten");
}

// Disconnect from current WiFi
static void wifi_disconnect_btn_cb(lv_event_t *e) {
  (void)e;
  ESP_LOGI(WIFI_TAG, "Disconnecting from WiFi...");

  esp_wifi_disconnect();
  wifi_connected = false;

  if (lvgl_port_lock(10)) {
    if (wifi_status_label) {
      lv_label_set_text(wifi_status_label,
                        "Deconnecte. Scannez pour reconnecter.");
    }
    if (icon_wifi) {
      lv_obj_set_style_text_color(icon_wifi, COLOR_TEXT_DIM, 0);
    }
    lvgl_port_unlock();
  }

  memset(wifi_ip, 0, sizeof(wifi_ip));
  ESP_LOGI(WIFI_TAG, "WiFi disconnected");
}

static void nav_wifi_cb(lv_event_t *e) {
  (void)e;
  show_page(page_wifi);
}

// ====================================================================================
// BLUETOOTH PAGE CALLBACKS
// ====================================================================================

// Forward declarations for Bluetooth callbacks
static void bt_scan_btn_cb(lv_event_t *e);
static void bt_list_cb(lv_event_t *e);
static void bt_back_btn_cb(lv_event_t *e);

#if CONFIG_BT_ENABLED
// Update Bluetooth device list from scan results
// Show up to 10 devices
#define BT_MAX_DISPLAY_DEVICES 10

static void update_bt_list(void) {
  if (!bt_list)
    return;
  lv_obj_clean(bt_list);

  int displayed = 0;
  for (int i = 0; i < bt_scan_count && i < BT_SCAN_MAX_DEVICES &&
                  displayed < BT_MAX_DISPLAY_DEVICES;
       i++) {
    if (!bt_scan_results[i].valid)
      continue;

    // Create list item with device name AND MAC address for identification
    char item_text[80];
    char bda_str[18];
    bda_to_str(bt_scan_results[i].bda, bda_str, sizeof(bda_str));

    // Show name + MAC, or just MAC if name is unknown
    if (strcmp(bt_scan_results[i].name, "(Unknown)") == 0 ||
        strlen(bt_scan_results[i].name) == 0) {
      // No name - show MAC with "Inconnu" label
      snprintf(item_text, sizeof(item_text), "Inconnu (%s)", bda_str);
    } else {
      // Has name - show name + short MAC (last 8 chars)
      const char *short_mac =
          strlen(bda_str) > 8 ? bda_str + strlen(bda_str) - 8 : bda_str;
      snprintf(item_text, sizeof(item_text), "%s (...%s)",
               bt_scan_results[i].name, short_mac);
    }

    lv_obj_t *btn = lv_list_add_button(bt_list, LV_SYMBOL_BLUETOOTH, item_text);
    lv_obj_add_event_cb(btn, bt_list_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    lv_obj_set_style_bg_color(btn, COLOR_BG_CARD, 0);
    lv_obj_set_style_text_color(btn, COLOR_TEXT, 0);

    // Add RSSI indicator with signal quality
    char rssi_str[24];
    int rssi = bt_scan_results[i].rssi;
    const char *quality =
        rssi > -60 ? "Fort" : (rssi > -80 ? "Moyen" : "Faible");
    snprintf(rssi_str, sizeof(rssi_str), "%s (%d)", quality, rssi);
    lv_obj_t *rssi_label = lv_label_create(btn);
    lv_label_set_text(rssi_label, rssi_str);
    lv_obj_set_style_text_color(
        rssi_label,
        rssi > -60 ? COLOR_SUCCESS
                   : (rssi > -80 ? COLOR_WARNING : COLOR_TEXT_DIM),
        0);
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_12, 0);
    lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, -10, 0);

    displayed++;
  }
}

// Timer callback to update BT list after scan
static void bt_scan_timer_cb(lv_timer_t *timer) {
  // Use LVGL lock for thread safety
  if (!lvgl_port_lock(100)) {
    ESP_LOGW(BT_TAG, "Could not acquire LVGL lock for BT list update");
    lv_timer_delete(timer);
    return;
  }

  // Update the list with results
  update_bt_list();

  if (bt_status_label) {
    char status[64];
    snprintf(status, sizeof(status), "%d appareils BLE trouves", bt_scan_count);
    lv_label_set_text(bt_status_label, status);
  }

  lvgl_port_unlock();

  // Delete the timer
  lv_timer_delete(timer);
}
#endif

static void bt_scan_btn_cb(lv_event_t *e) {
  (void)e;
  if (bt_status_label) {
    lv_label_set_text(bt_status_label, "Recherche des appareils BLE...");
  }

#if CONFIG_BT_ENABLED
  // Start BLE scan (10 seconds) - will auto-stop if one is in progress
  // UI will be updated from main loop when bt_scan_update_pending becomes true
  esp_err_t ret = bluetooth_start_scan(10);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "BLE scan failed: %s", esp_err_to_name(ret));
    if (bt_status_label) {
      lv_label_set_text(bt_status_label, "Echec - reessayez");
    }
    return;
  }
  // No timer needed - main loop checks bt_scan_update_pending flag
#else
  // BT disabled in config
  if (bt_status_label) {
    lv_label_set_text(bt_status_label, "Bluetooth desactive");
  }
#endif
}

static void bt_list_cb(lv_event_t *e) {
  int idx = (int)(intptr_t)lv_event_get_user_data(e);
#if CONFIG_BT_ENABLED
  if (idx >= 0 && idx < bt_scan_count && bt_scan_results[idx].valid) {
    bt_selected_device_idx = idx; // Store selected device

    if (bt_device_label) {
      char info[128];
      char bda_str[18];
      bda_to_str(bt_scan_results[idx].bda, bda_str, sizeof(bda_str));
      snprintf(info, sizeof(info), "Appareil: %s\nMAC: %s\nRSSI: %d dBm",
               bt_scan_results[idx].name, bda_str, bt_scan_results[idx].rssi);
      lv_label_set_text(bt_device_label, info);
    }
    ESP_LOGI(TAG, "Selected BLE device [%d]: %s", idx,
             bt_scan_results[idx].name);
  }
#else
  (void)idx;
  if (bt_device_label) {
    lv_label_set_text(bt_device_label, "Bluetooth non disponible");
  }
#endif
}

static void bt_back_btn_cb(lv_event_t *e) {
  (void)e;
  show_page(page_settings);
}

static void nav_bluetooth_cb(lv_event_t *e) {
  (void)e;
  show_page(page_bluetooth);
}

// ====================================================================================
// ESP32-C6 OTA UPDATE CALLBACKS
// ====================================================================================

static lv_obj_t *ota_msgbox = NULL;

static void ota_update_task(void *arg) {
  ESP_LOGI(OTA_TAG, "Starting OTA update task...");

  esp_err_t ret = perform_c6_ota_update();

  if (ret == ESP_OK) {
    ESP_LOGI(OTA_TAG, "OTA completed successfully!");
    // The C6 will reboot, we need to wait for reconnection
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(OTA_TAG, "C6 should be rebooting with new firmware...");
  } else {
    ESP_LOGE(OTA_TAG, "OTA failed: %s", esp_err_to_name(ret));
  }

  vTaskDelete(NULL);
}

static void ota_confirm_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);

  // Get button label text
  lv_obj_t *label = lv_obj_get_child(btn, 0);
  if (!label)
    return;

  const char *btn_text = lv_label_get_text(label);

  if (strcmp(btn_text, "Oui") == 0) {
    ESP_LOGI(OTA_TAG, "User confirmed OTA update");

    // Close confirmation dialog
    if (ota_msgbox) {
      lv_msgbox_close(ota_msgbox);
      ota_msgbox = NULL;
    }

    // Show progress message
    ota_msgbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(ota_msgbox, LV_SYMBOL_DOWNLOAD " Mise a jour C6");
    lv_msgbox_add_text(ota_msgbox, "Mise a jour en cours...\nNe pas eteindre!");
    lv_obj_center(ota_msgbox);

    // Start OTA in a separate task
    xTaskCreate(ota_update_task, "ota_task", 8192, NULL, 5, NULL);
  } else if (strcmp(btn_text, "Non") == 0) {
    ESP_LOGI(OTA_TAG, "User cancelled OTA update");
    if (ota_msgbox) {
      lv_msgbox_close(ota_msgbox);
      ota_msgbox = NULL;
    }
  }
}

static void c6_ota_btn_cb(lv_event_t *e) {
  (void)e;
  ESP_LOGI(OTA_TAG, "C6 OTA button clicked");

  if (ota_in_progress) {
    ESP_LOGW(OTA_TAG, "OTA already in progress");
    return;
  }

  // Show confirmation dialog
  ota_msgbox = lv_msgbox_create(NULL);
  lv_msgbox_add_title(ota_msgbox, LV_SYMBOL_WARNING " Mise a jour ESP32-C6");
  lv_msgbox_add_text(ota_msgbox,
                     "Voulez-vous mettre a jour\nle firmware du module "
                     "WiFi/BT?\n\nCela prendra environ 30 secondes.");

  // Add buttons and attach click events
  lv_obj_t *btn_oui = lv_msgbox_add_footer_button(ota_msgbox, "Oui");
  lv_obj_t *btn_non = lv_msgbox_add_footer_button(ota_msgbox, "Non");

  lv_obj_add_event_cb(btn_oui, ota_confirm_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(btn_non, ota_confirm_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_center(ota_msgbox);
}

// ====================================================================================
// CREATE STATUS BAR
// ====================================================================================

static void create_status_bar(lv_obj_t *parent) {
  lv_obj_t *status_bar = lv_obj_create(parent);
  lv_obj_set_size(status_bar, LCD_H_RES, 50);
  lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(status_bar, COLOR_HEADER, 0);
  lv_obj_set_style_bg_opa(status_bar, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(status_bar, 0, 0);
  lv_obj_set_style_radius(status_bar, 0, 0);
  lv_obj_set_style_pad_hor(status_bar, 12, 0);
  lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

  // Left: Logo from SD or fallback icon + "Smart Panel"
  lv_obj_t *logo_container = lv_obj_create(status_bar);
  lv_obj_set_size(logo_container, 180, 40);
  lv_obj_align(logo_container, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_bg_opa(logo_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(logo_container, 0, 0);
  lv_obj_set_style_pad_all(logo_container, 0, 0);
  lv_obj_clear_flag(logo_container, LV_OBJ_FLAG_SCROLLABLE);

  // Try to load logo from SD card
  if (sd_mounted) {
    logo_img = lv_image_create(logo_container);
    lv_image_set_src(logo_img, SD_MOUNT_POINT "/imgs/logo.png");
    lv_obj_set_size(logo_img, 32, 32);
    lv_image_set_inner_align(logo_img, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(logo_img, LV_ALIGN_LEFT_MID, 0, 0);

    // Check if image loaded successfully
    if (lv_image_get_src(logo_img) == NULL) {
      ESP_LOGW(TAG, "Failed to load logo, using fallback");
      lv_obj_delete(logo_img);
      logo_img = lv_label_create(logo_container);
      lv_label_set_text(logo_img, LV_SYMBOL_HOME);
      lv_obj_set_style_text_color(logo_img, COLOR_PRIMARY, 0);
      lv_obj_set_style_text_font(logo_img, &lv_font_montserrat_24, 0);
      lv_obj_align(logo_img, LV_ALIGN_LEFT_MID, 0, 0);
    }
  } else {
    // Fallback icon
    logo_img = lv_label_create(logo_container);
    lv_label_set_text(logo_img, LV_SYMBOL_HOME);
    lv_obj_set_style_text_color(logo_img, COLOR_PRIMARY, 0);
    lv_obj_set_style_text_font(logo_img, &lv_font_montserrat_24, 0);
    lv_obj_align(logo_img, LV_ALIGN_LEFT_MID, 0, 0);
  }

  lv_obj_t *title = lv_label_create(logo_container);
  lv_label_set_text(title, "Smart Panel");
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
  lv_obj_align(title, LV_ALIGN_LEFT_MID, 38, 0);

  // Right: Date, Time, BT, WiFi
  lv_obj_t *right_container = lv_obj_create(status_bar);
  lv_obj_set_size(right_container, 260, 40);
  lv_obj_align(right_container, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_opa(right_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(right_container, 0, 0);
  lv_obj_set_style_pad_all(right_container, 0, 0);
  lv_obj_clear_flag(right_container, LV_OBJ_FLAG_SCROLLABLE);

  label_date = lv_label_create(right_container);
  lv_label_set_text(label_date, "01 Jan");
  lv_obj_set_style_text_color(label_date, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(label_date, &lv_font_montserrat_12, 0);
  lv_obj_align(label_date, LV_ALIGN_LEFT_MID, 0, 0);

  label_time = lv_label_create(right_container);
  lv_label_set_text(label_time, "00:00");
  lv_obj_set_style_text_color(label_time, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(label_time, &lv_font_montserrat_16, 0);
  lv_obj_align(label_time, LV_ALIGN_LEFT_MID, 55, 0);

  icon_bluetooth = lv_label_create(right_container);
  lv_label_set_text(icon_bluetooth, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(
      icon_bluetooth, bluetooth_enabled ? COLOR_PRIMARY : COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(icon_bluetooth, &lv_font_montserrat_18, 0);
  lv_obj_align(icon_bluetooth, LV_ALIGN_RIGHT_MID, -30, 0);

  icon_wifi = lv_label_create(right_container);
  lv_label_set_text(icon_wifi, LV_SYMBOL_WIFI);
  lv_obj_set_style_text_color(icon_wifi,
                              wifi_enabled ? COLOR_SUCCESS : COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(icon_wifi, &lv_font_montserrat_18, 0);
  lv_obj_align(icon_wifi, LV_ALIGN_RIGHT_MID, 0, 0);
}

// ====================================================================================
// CREATE NAVIGATION BAR
// ====================================================================================

static void create_navbar(lv_obj_t *parent) {
  lv_obj_t *navbar = lv_obj_create(parent);
  lv_obj_set_size(navbar, LCD_H_RES, 70);
  lv_obj_align(navbar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(navbar, COLOR_HEADER, 0);
  lv_obj_set_style_border_width(navbar, 0, 0);
  lv_obj_set_style_radius(navbar, 0, 0);
  lv_obj_set_style_pad_all(navbar, 0, 0);
  lv_obj_set_flex_flow(navbar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(navbar, LV_FLEX_ALIGN_SPACE_EVENLY,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(navbar, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *btn_home = lv_btn_create(navbar);
  lv_obj_set_size(btn_home, 140, 50);
  lv_obj_set_style_bg_color(btn_home, COLOR_ACCENT, 0);
  lv_obj_set_style_radius(btn_home, 12, 0);
  lv_obj_set_style_shadow_width(btn_home, 0, 0);
  lv_obj_t *lbl_home = lv_label_create(btn_home);
  lv_label_set_text(lbl_home, LV_SYMBOL_HOME " Home");
  lv_obj_set_style_text_font(lbl_home, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(lbl_home, COLOR_TEXT, 0);
  lv_obj_center(lbl_home);
  lv_obj_add_event_cb(btn_home, nav_home_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *btn_settings = lv_btn_create(navbar);
  lv_obj_set_size(btn_settings, 140, 50);
  lv_obj_set_style_bg_color(btn_settings, COLOR_ACCENT, 0);
  lv_obj_set_style_radius(btn_settings, 12, 0);
  lv_obj_set_style_shadow_width(btn_settings, 0, 0);
  lv_obj_t *lbl_settings = lv_label_create(btn_settings);
  lv_label_set_text(lbl_settings, LV_SYMBOL_SETTINGS " Settings");
  lv_obj_set_style_text_font(lbl_settings, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(lbl_settings, COLOR_TEXT, 0);
  lv_obj_center(lbl_settings);
  lv_obj_add_event_cb(btn_settings, nav_settings_cb, LV_EVENT_CLICKED, NULL);
}

// ====================================================================================
// CREATE PAGES
// ====================================================================================

static void create_home_page(lv_obj_t *parent) {
  page_home = lv_obj_create(parent);
  lv_obj_set_size(page_home, LCD_H_RES, LCD_V_RES - 120);
  lv_obj_align(page_home, LV_ALIGN_TOP_MID, 0, 50);
  lv_obj_set_style_bg_color(page_home, COLOR_BG_DARK, 0);
  lv_obj_set_style_border_width(page_home, 0, 0);
  lv_obj_set_style_radius(page_home, 0, 0);
  lv_obj_set_style_pad_all(page_home, 16, 0);
  lv_obj_set_flex_flow(page_home, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(page_home, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(page_home, 10, 0);

  // Quick Actions Row
  lv_obj_t *quick_row = lv_obj_create(page_home);
  lv_obj_set_size(quick_row, LCD_H_RES - 32, 90);
  lv_obj_set_style_bg_opa(quick_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(quick_row, 0, 0);
  lv_obj_set_style_pad_all(quick_row, 0, 0);
  lv_obj_set_flex_flow(quick_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(quick_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(quick_row, LV_OBJ_FLAG_SCROLLABLE);

  // WiFi Card
  lv_obj_t *wifi_card = create_card(quick_row, 135, 80);
  lv_obj_t *wifi_icon_home = lv_label_create(wifi_card);
  lv_label_set_text(wifi_icon_home, LV_SYMBOL_WIFI);
  lv_obj_set_style_text_color(wifi_icon_home,
                              wifi_connected ? COLOR_SUCCESS : COLOR_DANGER, 0);
  lv_obj_set_style_text_font(wifi_icon_home, &lv_font_montserrat_24, 0);
  lv_obj_align(wifi_icon_home, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_t *wifi_lbl = lv_label_create(wifi_card);
  lv_label_set_text(wifi_lbl, wifi_connected ? "Connected" : "Offline");
  lv_obj_set_style_text_color(wifi_lbl, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(wifi_lbl, &lv_font_montserrat_12, 0);
  lv_obj_align(wifi_lbl, LV_ALIGN_BOTTOM_MID, 0, 0);

  // Bluetooth Card
  lv_obj_t *bt_card = create_card(quick_row, 135, 80);
  lv_obj_t *bt_icon = lv_label_create(bt_card);
  lv_label_set_text(bt_icon, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(
      bt_icon, bluetooth_enabled ? COLOR_PRIMARY : COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(bt_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(bt_icon, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_t *bt_lbl = lv_label_create(bt_card);
  lv_label_set_text(bt_lbl, bluetooth_enabled ? "Enabled" : "Off");
  lv_obj_set_style_text_color(bt_lbl, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(bt_lbl, &lv_font_montserrat_12, 0);
  lv_obj_align(bt_lbl, LV_ALIGN_BOTTOM_MID, 0, 0);

  // SD Card Status
  lv_obj_t *sd_card_ui = create_card(quick_row, 135, 80);
  lv_obj_t *sd_icon = lv_label_create(sd_card_ui);
  lv_label_set_text(sd_icon, LV_SYMBOL_SD_CARD);
  lv_obj_set_style_text_color(sd_icon,
                              sd_mounted ? COLOR_SUCCESS : COLOR_DANGER, 0);
  lv_obj_set_style_text_font(sd_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(sd_icon, LV_ALIGN_TOP_MID, 0, 0);
  sd_status_label = lv_label_create(sd_card_ui);
  lv_label_set_text(sd_status_label, sd_mounted ? "Mounted" : "No Card");
  lv_obj_set_style_text_color(sd_status_label, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(sd_status_label, &lv_font_montserrat_12, 0);
  lv_obj_align(sd_status_label, LV_ALIGN_BOTTOM_MID, 0, 0);

  // Brightness Card
  lv_obj_t *bright_card = create_card(page_home, LCD_H_RES - 32, 90);
  lv_obj_t *bright_title = lv_label_create(bright_card);
  lv_label_set_text(bright_title, LV_SYMBOL_IMAGE " Brightness");
  lv_obj_set_style_text_color(bright_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(bright_title, &lv_font_montserrat_14, 0);
  lv_obj_align(bright_title, LV_ALIGN_TOP_LEFT, 0, 0);

  slider_brightness = lv_slider_create(bright_card);
  lv_obj_set_width(slider_brightness, LCD_H_RES - 80);
  lv_obj_align(slider_brightness, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_slider_set_range(slider_brightness, 10, 100);
  lv_slider_set_value(slider_brightness, current_brightness, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(slider_brightness, COLOR_ACCENT, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_brightness, COLOR_PRIMARY,
                            LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_brightness, COLOR_TEXT, LV_PART_KNOB);
  lv_obj_add_event_cb(slider_brightness, brightness_cb, LV_EVENT_VALUE_CHANGED,
                      NULL);

  // System Info Card
  lv_obj_t *info_card = create_card(page_home, LCD_H_RES - 32, 200);
  lv_obj_t *info_title = lv_label_create(info_card);
  lv_label_set_text(info_title, LV_SYMBOL_FILE " System Information");
  lv_obj_set_style_text_color(info_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(info_title, &lv_font_montserrat_14, 0);
  lv_obj_align(info_title, LV_ALIGN_TOP_LEFT, 0, 0);

  char info_buf[300];
  snprintf(info_buf, sizeof(info_buf),
           "Board:     GUITION JC4880P443C\n"
           "MCU:       ESP32-P4 RISC-V 360MHz\n"
           "Co-Proc:   ESP32-C6 (WiFi/BLE)\n"
           "Display:   480x800 MIPI-DSI\n"
           "Touch:     GT911 Capacitive\n"
           "Memory:    32MB PSRAM + 16MB Flash\n"
           "SD Card:   %s",
           sd_mounted ? "Mounted" : "Not mounted");

  lv_obj_t *info_text = lv_label_create(info_card);
  lv_label_set_text(info_text, info_buf);
  lv_obj_set_style_text_color(info_text, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(info_text, &lv_font_montserrat_12, 0);
  lv_obj_align(info_text, LV_ALIGN_TOP_LEFT, 0, 28);
}

static void create_settings_page(lv_obj_t *parent) {
  page_settings = lv_obj_create(parent);
  lv_obj_set_size(page_settings, LCD_H_RES, LCD_V_RES - 120);
  lv_obj_align(page_settings, LV_ALIGN_TOP_MID, 0, 50);
  lv_obj_set_style_bg_color(page_settings, COLOR_BG_DARK, 0);
  lv_obj_set_style_border_width(page_settings, 0, 0);
  lv_obj_set_style_radius(page_settings, 0, 0);
  lv_obj_set_style_pad_all(page_settings, 16, 0);
  lv_obj_set_flex_flow(page_settings, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(page_settings, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(page_settings, 10, 0);
  lv_obj_add_flag(page_settings, LV_OBJ_FLAG_HIDDEN);

  // Connectivity Card
  lv_obj_t *conn_card = create_card(page_settings, LCD_H_RES - 32, 130);
  lv_obj_t *conn_title = lv_label_create(conn_card);
  lv_label_set_text(conn_title, LV_SYMBOL_WIFI " Connectivity");
  lv_obj_set_style_text_color(conn_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(conn_title, &lv_font_montserrat_14, 0);
  lv_obj_align(conn_title, LV_ALIGN_TOP_LEFT, 0, 0);

  // WiFi Row
  lv_obj_t *wifi_row = lv_obj_create(conn_card);
  lv_obj_set_size(wifi_row, LCD_H_RES - 80, 32);
  lv_obj_align(wifi_row, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_obj_set_style_bg_opa(wifi_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(wifi_row, 0, 0);
  lv_obj_set_style_pad_all(wifi_row, 0, 0);
  lv_obj_clear_flag(wifi_row, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *wifi_lbl = lv_label_create(wifi_row);
  lv_label_set_text(wifi_lbl, LV_SYMBOL_WIFI "  WiFi (ESP32-C6)");
  lv_obj_set_style_text_color(wifi_lbl, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(wifi_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(wifi_lbl, LV_ALIGN_LEFT_MID, 0, 0);

  // WiFi Settings button
  lv_obj_t *wifi_settings_btn = lv_button_create(wifi_row);
  lv_obj_set_size(wifi_settings_btn, 80, 28);
  lv_obj_align(wifi_settings_btn, LV_ALIGN_RIGHT_MID, -60, 0);
  lv_obj_set_style_bg_color(wifi_settings_btn, COLOR_PRIMARY, 0);
  lv_obj_set_style_radius(wifi_settings_btn, 6, 0);
  lv_obj_add_event_cb(wifi_settings_btn, nav_wifi_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *wifi_btn_lbl = lv_label_create(wifi_settings_btn);
  lv_label_set_text(wifi_btn_lbl, LV_SYMBOL_SETTINGS);
  lv_obj_center(wifi_btn_lbl);

  lv_obj_t *wifi_sw = lv_switch_create(wifi_row);
  lv_obj_align(wifi_sw, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(wifi_sw, COLOR_ACCENT, LV_PART_MAIN);
  lv_obj_set_style_bg_color(wifi_sw, COLOR_SUCCESS,
                            LV_PART_INDICATOR | LV_STATE_CHECKED);
  if (wifi_enabled)
    lv_obj_add_state(wifi_sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(wifi_sw, wifi_toggle_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // Bluetooth Row
  lv_obj_t *bt_row = lv_obj_create(conn_card);
  lv_obj_set_size(bt_row, LCD_H_RES - 80, 32);
  lv_obj_align(bt_row, LV_ALIGN_TOP_LEFT, 0, 68);
  lv_obj_set_style_bg_opa(bt_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(bt_row, 0, 0);
  lv_obj_set_style_pad_all(bt_row, 0, 0);
  lv_obj_clear_flag(bt_row, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *bt_lbl = lv_label_create(bt_row);
  lv_label_set_text(bt_lbl, LV_SYMBOL_BLUETOOTH "  Bluetooth");
  lv_obj_set_style_text_color(bt_lbl, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(bt_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(bt_lbl, LV_ALIGN_LEFT_MID, 0, 0);

  // Bluetooth Settings button
  lv_obj_t *bt_settings_btn = lv_button_create(bt_row);
  lv_obj_set_size(bt_settings_btn, 80, 28);
  lv_obj_align(bt_settings_btn, LV_ALIGN_RIGHT_MID, -60, 0);
  lv_obj_set_style_bg_color(bt_settings_btn, COLOR_PRIMARY, 0);
  lv_obj_set_style_radius(bt_settings_btn, 6, 0);
  lv_obj_add_event_cb(bt_settings_btn, nav_bluetooth_cb, LV_EVENT_CLICKED,
                      NULL);
  lv_obj_t *bt_btn_lbl = lv_label_create(bt_settings_btn);
  lv_label_set_text(bt_btn_lbl, LV_SYMBOL_SETTINGS);
  lv_obj_center(bt_btn_lbl);

  lv_obj_t *bt_sw = lv_switch_create(bt_row);
  lv_obj_align(bt_sw, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(bt_sw, COLOR_ACCENT, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bt_sw, COLOR_PRIMARY,
                            LV_PART_INDICATOR | LV_STATE_CHECKED);
  if (bluetooth_enabled)
    lv_obj_add_state(bt_sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(bt_sw, bluetooth_toggle_cb, LV_EVENT_VALUE_CHANGED, NULL);

  // Display Card
  lv_obj_t *disp_card = create_card(page_settings, LCD_H_RES - 32, 90);
  lv_obj_t *disp_title = lv_label_create(disp_card);
  lv_label_set_text(disp_title, LV_SYMBOL_IMAGE " Display");
  lv_obj_set_style_text_color(disp_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(disp_title, &lv_font_montserrat_14, 0);
  lv_obj_align(disp_title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *disp_info = lv_label_create(disp_card);
  lv_label_set_text_fmt(disp_info, "Resolution: 480 x 800  |  Brightness: %d%%",
                        current_brightness);
  lv_obj_set_style_text_color(disp_info, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(disp_info, &lv_font_montserrat_12, 0);
  lv_obj_align(disp_info, LV_ALIGN_TOP_LEFT, 0, 30);

  // Storage Card
  lv_obj_t *storage_card = create_card(page_settings, LCD_H_RES - 32, 90);
  lv_obj_t *storage_title = lv_label_create(storage_card);
  lv_label_set_text(storage_title, LV_SYMBOL_SD_CARD " Storage");
  lv_obj_set_style_text_color(storage_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(storage_title, &lv_font_montserrat_14, 0);
  lv_obj_align(storage_title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *storage_info = lv_label_create(storage_card);
  if (sd_mounted && sd_card) {
    lv_label_set_text_fmt(storage_info, "SD Card: %s\nCapacity: %llu MB",
                          sd_card->cid.name,
                          (uint64_t)sd_card->csd.capacity *
                              sd_card->csd.sector_size / (1024 * 1024));
  } else {
    lv_label_set_text(storage_info, "SD Card: Not mounted");
  }
  lv_obj_set_style_text_color(storage_info, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(storage_info, &lv_font_montserrat_12, 0);
  lv_obj_align(storage_info, LV_ALIGN_TOP_LEFT, 0, 30);

  // About Card
  lv_obj_t *about_card = create_card(page_settings, LCD_H_RES - 32, 150);
  lv_obj_t *about_title = lv_label_create(about_card);
  lv_label_set_text(about_title, LV_SYMBOL_FILE " About");
  lv_obj_set_style_text_color(about_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(about_title, &lv_font_montserrat_14, 0);
  lv_obj_align(about_title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *about_text = lv_label_create(about_card);
  lv_label_set_text(about_text, "Smart Panel Demo v1.0\n\n"
                                "ESP-IDF:  v6.1-dev\n"
                                "LVGL:     v9.4\n"
                                "ESP-Hosted: v2.8.5\n"
                                "© 2026 IoT Development");
  lv_obj_set_style_text_color(about_text, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(about_text, &lv_font_montserrat_12, 0);
  lv_obj_align(about_text, LV_ALIGN_TOP_LEFT, 0, 28);

  // Firmware Update Card
  lv_obj_t *update_card = create_card(page_settings, LCD_H_RES - 32, 100);
  lv_obj_t *update_title = lv_label_create(update_card);
  lv_label_set_text(update_title, LV_SYMBOL_DOWNLOAD " Mise a jour Firmware");
  lv_obj_set_style_text_color(update_title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(update_title, &lv_font_montserrat_14, 0);
  lv_obj_align(update_title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *update_info = lv_label_create(update_card);
  lv_label_set_text(update_info, "ESP32-C6 (WiFi/Bluetooth)");
  lv_obj_set_style_text_color(update_info, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(update_info, &lv_font_montserrat_12, 0);
  lv_obj_align(update_info, LV_ALIGN_TOP_LEFT, 0, 25);

  // Update C6 Button
  lv_obj_t *update_c6_btn = lv_button_create(update_card);
  lv_obj_set_size(update_c6_btn, 180, 40);
  lv_obj_align(update_c6_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_bg_color(update_c6_btn, COLOR_ACCENT, 0);
  lv_obj_set_style_radius(update_c6_btn, 8, 0);
  lv_obj_add_event_cb(update_c6_btn, c6_ota_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *update_c6_label = lv_label_create(update_c6_btn);
  lv_label_set_text(update_c6_label, LV_SYMBOL_REFRESH " Mettre a jour C6");
  lv_obj_center(update_c6_label);
}

// ====================================================================================
// CREATE WIFI PAGE
// ====================================================================================

static void create_wifi_page(lv_obj_t *parent) {
  page_wifi = lv_obj_create(parent);
  lv_obj_set_size(page_wifi, LCD_H_RES, LCD_V_RES - 50 - 60);
  lv_obj_set_pos(page_wifi, 0, 50);
  lv_obj_set_style_bg_color(page_wifi, COLOR_BG_DARK, 0);
  lv_obj_set_style_border_width(page_wifi, 0, 0);
  lv_obj_set_style_pad_all(page_wifi, 10, 0);
  lv_obj_add_flag(page_wifi, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_flex_flow(page_wifi, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(page_wifi, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(page_wifi, 8, 0);

  // Header with back button
  lv_obj_t *header = lv_obj_create(page_wifi);
  lv_obj_set_size(header, LCD_H_RES - 20, 50);
  lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_pad_all(header, 0, 0);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *back_btn = lv_button_create(header);
  lv_obj_set_size(back_btn, 50, 40);
  lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_bg_color(back_btn, COLOR_ACCENT, 0);
  lv_obj_add_event_cb(back_btn, wifi_back_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *back_lbl = lv_label_create(back_btn);
  lv_label_set_text(back_lbl, LV_SYMBOL_LEFT);
  lv_obj_center(back_lbl);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "WiFi Configuration");
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *scan_btn = lv_button_create(header);
  lv_obj_set_size(scan_btn, 80, 40);
  lv_obj_align(scan_btn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(scan_btn, COLOR_PRIMARY, 0);
  lv_obj_add_event_cb(scan_btn, wifi_scan_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *scan_lbl = lv_label_create(scan_btn);
  lv_label_set_text(scan_lbl, "Scan");
  lv_obj_center(scan_lbl);

  // Current Network Card (shown when connected)
  lv_obj_t *current_net_card = lv_obj_create(page_wifi);
  lv_obj_set_size(current_net_card, LCD_H_RES - 20, 100);
  lv_obj_set_style_bg_color(current_net_card, COLOR_BG_CARD, 0);
  lv_obj_set_style_border_color(current_net_card, COLOR_SUCCESS, 0);
  lv_obj_set_style_border_width(current_net_card, 2, 0);
  lv_obj_set_style_radius(current_net_card, 12, 0);
  lv_obj_set_style_pad_all(current_net_card, 10, 0);
  lv_obj_clear_flag(current_net_card, LV_OBJ_FLAG_SCROLLABLE);

  // Card Title - "Reseau actuel"
  lv_obj_t *net_title = lv_label_create(current_net_card);
  lv_label_set_text(net_title, LV_SYMBOL_WIFI " Reseau actuel");
  lv_obj_set_style_text_color(net_title, COLOR_SUCCESS, 0);
  lv_obj_set_style_text_font(net_title, &lv_font_montserrat_14, 0);
  lv_obj_align(net_title, LV_ALIGN_TOP_LEFT, 0, 0);

  // Network name and IP
  lv_obj_t *net_info = lv_label_create(current_net_card);
  if (wifi_connected && strlen(wifi_selected_ssid) > 0) {
    char info_buf[96];
    snprintf(info_buf, sizeof(info_buf), "%s\nIP: %s", wifi_selected_ssid,
             wifi_ip);
    lv_label_set_text(net_info, info_buf);
  } else {
    lv_label_set_text(net_info, "Non connecte");
  }
  lv_obj_set_style_text_color(net_info, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(net_info, &lv_font_montserrat_12, 0);
  lv_obj_align(net_info, LV_ALIGN_TOP_LEFT, 0, 22);

  // Action buttons row
  lv_obj_t *disconnect_btn = lv_button_create(current_net_card);
  lv_obj_set_size(disconnect_btn, 100, 30);
  lv_obj_align(disconnect_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_set_style_bg_color(disconnect_btn, lv_color_hex(0xFF9800), 0);
  lv_obj_set_style_radius(disconnect_btn, 6, 0);
  lv_obj_add_event_cb(disconnect_btn, wifi_disconnect_btn_cb, LV_EVENT_CLICKED,
                      NULL);
  lv_obj_t *disc_lbl = lv_label_create(disconnect_btn);
  lv_label_set_text(disc_lbl, "Deconnecter");
  lv_obj_set_style_text_font(disc_lbl, &lv_font_montserrat_12, 0);
  lv_obj_center(disc_lbl);

  lv_obj_t *forget_btn2 = lv_button_create(current_net_card);
  lv_obj_set_size(forget_btn2, 80, 30);
  lv_obj_align(forget_btn2, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_set_style_bg_color(forget_btn2, COLOR_DANGER, 0);
  lv_obj_set_style_radius(forget_btn2, 6, 0);
  lv_obj_add_event_cb(forget_btn2, wifi_forget_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *forg_lbl = lv_label_create(forget_btn2);
  lv_label_set_text(forg_lbl, "Oublier");
  lv_obj_set_style_text_font(forg_lbl, &lv_font_montserrat_12, 0);
  lv_obj_center(forg_lbl);

  // Hide card if not connected
  if (!wifi_connected) {
    lv_obj_add_flag(current_net_card, LV_OBJ_FLAG_HIDDEN);
  }

  // Status label
  wifi_status_label = lv_label_create(page_wifi);
  lv_label_set_text(wifi_status_label,
                    wifi_connected ? "Connecte - Scannez pour d'autres reseaux"
                                   : "Scannez pour trouver des reseaux");
  lv_obj_set_style_text_color(wifi_status_label, COLOR_TEXT_DIM, 0);

  // Selected SSID label
  wifi_ssid_label = lv_label_create(page_wifi);
  lv_label_set_text(wifi_ssid_label, "Reseau: (aucun selectionne)");
  lv_obj_set_style_text_color(wifi_ssid_label, COLOR_SUCCESS, 0);

  // WiFi network list - reduced height to make room for bigger keyboard
  wifi_list = lv_list_create(page_wifi);
  lv_obj_set_size(wifi_list, LCD_H_RES - 20, 140);
  lv_obj_set_style_bg_color(wifi_list, COLOR_BG_CARD, 0);
  lv_obj_set_style_border_color(wifi_list, COLOR_BORDER, 0);
  lv_obj_set_style_radius(wifi_list, 12, 0);

  // Password container (textarea + eye button)
  lv_obj_t *pwd_container = lv_obj_create(page_wifi);
  lv_obj_set_size(pwd_container, LCD_H_RES - 20, 50);
  lv_obj_set_style_bg_opa(pwd_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(pwd_container, 0, 0);
  lv_obj_set_style_pad_all(pwd_container, 0, 0);
  lv_obj_clear_flag(pwd_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(pwd_container, LV_OBJ_FLAG_HIDDEN);
  wifi_pwd_container = pwd_container; // Store reference for callbacks

  // Password text area
  wifi_password_ta = lv_textarea_create(pwd_container);
  lv_obj_set_size(wifi_password_ta, LCD_H_RES - 80, 45);
  lv_obj_align(wifi_password_ta, LV_ALIGN_LEFT_MID, 0, 0);
  lv_textarea_set_placeholder_text(wifi_password_ta, "Password...");
  lv_textarea_set_password_mode(wifi_password_ta, true);
  lv_textarea_set_one_line(wifi_password_ta, true);
  lv_obj_set_style_bg_color(wifi_password_ta, COLOR_BG_CARD, 0);
  lv_obj_set_style_text_color(wifi_password_ta, COLOR_TEXT, 0);
  lv_obj_set_style_border_color(wifi_password_ta, COLOR_BORDER, 0);
  lv_obj_set_style_radius(wifi_password_ta, 8, 0);

  // Eye button to toggle password visibility
  lv_obj_t *eye_btn = lv_button_create(pwd_container);
  lv_obj_set_size(eye_btn, 50, 45);
  lv_obj_align(eye_btn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(eye_btn, COLOR_ACCENT, 0);
  lv_obj_set_style_radius(eye_btn, 8, 0);
  lv_obj_add_event_cb(eye_btn, wifi_password_toggle_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *eye_lbl = lv_label_create(eye_btn);
  lv_label_set_text(eye_lbl, LV_SYMBOL_EYE_CLOSE);
  lv_obj_set_style_text_font(eye_lbl, &lv_font_montserrat_18, 0);
  lv_obj_center(eye_lbl);

  // Connect button
  lv_obj_t *connect_btn = lv_button_create(page_wifi);
  lv_obj_set_size(connect_btn, 200, 45);
  lv_obj_set_style_bg_color(connect_btn, COLOR_SUCCESS, 0);
  lv_obj_set_style_radius(connect_btn, 8, 0);
  lv_obj_add_event_cb(connect_btn, wifi_connect_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *connect_lbl = lv_label_create(connect_btn);
  lv_label_set_text(connect_lbl, LV_SYMBOL_WIFI " Connecter");
  lv_obj_center(connect_lbl);

  // AZERTY Keyboard - BIGGER for easier use
  wifi_keyboard = lv_keyboard_create(page_wifi);
  lv_obj_set_size(wifi_keyboard, LCD_H_RES, 320);
  lv_keyboard_set_textarea(wifi_keyboard, wifi_password_ta);

  // Set custom AZERTY maps for each mode
  lv_keyboard_set_map(wifi_keyboard, LV_KEYBOARD_MODE_TEXT_LOWER,
                      kb_map_azerty_lower, kb_ctrl_lower);
  lv_keyboard_set_map(wifi_keyboard, LV_KEYBOARD_MODE_TEXT_UPPER,
                      kb_map_azerty_upper, kb_ctrl_upper);
  lv_keyboard_set_map(wifi_keyboard, LV_KEYBOARD_MODE_SPECIAL, kb_map_special,
                      kb_ctrl_special);

  // Start in lowercase mode
  lv_keyboard_set_mode(wifi_keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

  // Style the keyboard
  lv_obj_set_style_bg_color(wifi_keyboard, COLOR_BG_CARD, 0);
  lv_obj_set_style_bg_color(wifi_keyboard, COLOR_ACCENT, LV_PART_ITEMS);
  lv_obj_set_style_text_color(wifi_keyboard, COLOR_TEXT, LV_PART_ITEMS);

  // Event handler for keyboard:
  // - LV_EVENT_READY: Triggered when OK button is pressed (handles WiFi
  // connection)
  // - Mode switching (ABC/abc/1#): Handled AUTOMATICALLY by LVGL's default
  // handler NOTE: Do NOT add LV_EVENT_VALUE_CHANGED or LV_EVENT_CLICKED
  // handlers for keys! LVGL's default keyboard handler processes key presses
  // and mode switches.
  lv_obj_add_event_cb(wifi_keyboard, wifi_keyboard_ready_cb, LV_EVENT_READY,
                      NULL);
  lv_obj_add_flag(wifi_keyboard, LV_OBJ_FLAG_HIDDEN);
}

// ====================================================================================
// CREATE BLUETOOTH PAGE
// ====================================================================================

static void create_bluetooth_page(lv_obj_t *parent) {
  page_bluetooth = lv_obj_create(parent);
  lv_obj_set_size(page_bluetooth, LCD_H_RES, LCD_V_RES - 50 - 60);
  lv_obj_set_pos(page_bluetooth, 0, 50);
  lv_obj_set_style_bg_color(page_bluetooth, COLOR_BG_DARK, 0);
  lv_obj_set_style_border_width(page_bluetooth, 0, 0);
  lv_obj_set_style_pad_all(page_bluetooth, 10, 0);
  lv_obj_add_flag(page_bluetooth, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_flex_flow(page_bluetooth, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(page_bluetooth, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(page_bluetooth, 8, 0);

  // Header with back button
  lv_obj_t *header = lv_obj_create(page_bluetooth);
  lv_obj_set_size(header, LCD_H_RES - 20, 50);
  lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_pad_all(header, 0, 0);
  lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *back_btn = lv_button_create(header);
  lv_obj_set_size(back_btn, 50, 40);
  lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_bg_color(back_btn, COLOR_ACCENT, 0);
  lv_obj_add_event_cb(back_btn, bt_back_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *back_lbl = lv_label_create(back_btn);
  lv_label_set_text(back_lbl, LV_SYMBOL_LEFT);
  lv_obj_center(back_lbl);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, LV_SYMBOL_BLUETOOTH " Bluetooth");
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *scan_btn = lv_button_create(header);
  lv_obj_set_size(scan_btn, 80, 40);
  lv_obj_align(scan_btn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(scan_btn, COLOR_PRIMARY, 0);
  lv_obj_add_event_cb(scan_btn, bt_scan_btn_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *scan_lbl = lv_label_create(scan_btn);
  lv_label_set_text(scan_lbl, "Rechercher");
  lv_obj_center(scan_lbl);

  // Status label
  bt_status_label = lv_label_create(page_bluetooth);
#if CONFIG_BT_ENABLED
  lv_label_set_text(bt_status_label,
                    "Appuyez sur 'Rechercher' pour trouver des appareils");
#else
  lv_label_set_text(bt_status_label,
                    "Bluetooth desactive dans la configuration");
#endif
  lv_obj_set_style_text_color(bt_status_label, COLOR_TEXT_DIM, 0);

  // Selected device info label
  bt_device_label = lv_label_create(page_bluetooth);
  lv_label_set_text(bt_device_label, "Appareil: (aucun selectionne)");
  lv_obj_set_style_text_color(bt_device_label, COLOR_PRIMARY, 0);
  lv_obj_set_style_text_font(bt_device_label, &lv_font_montserrat_14, 0);

  // BLE device list
  bt_list = lv_list_create(page_bluetooth);
  lv_obj_set_size(bt_list, LCD_H_RES - 20, 350);
  lv_obj_set_style_bg_color(bt_list, COLOR_BG_CARD, 0);
  lv_obj_set_style_border_color(bt_list, COLOR_BORDER, 0);
  lv_obj_set_style_radius(bt_list, 12, 0);

  // Info text at bottom - explain BLE limitation
  lv_obj_t *info_label = lv_label_create(page_bluetooth);
  lv_label_set_text(info_label, LV_SYMBOL_WARNING
                    " Mode BLE uniquement\n"
                    "Telephones/PC (Bluetooth Classic) non visibles.\n"
                    "Visible: montres, capteurs, ecouteurs...");
  lv_obj_set_style_text_color(info_label, lv_color_hex(0xFF9800), 0); // Orange
  lv_obj_set_style_text_font(info_label, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);
}

static void create_ui(void) {
  if (!lvgl_port_lock(1000))
    return;

  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, COLOR_BG_DARK, 0);

  create_status_bar(scr);
  create_navbar(scr);
  create_home_page(scr);
  create_settings_page(scr);
  create_wifi_page(scr);
  create_bluetooth_page(scr);

  show_page(page_home);

  lvgl_port_unlock();
  ESP_LOGI(TAG, "UI created");
}

static void update_status_bar(void) {
  static uint32_t secs = 0;
  secs++;

  if (lvgl_port_lock(10)) {
    if (label_time) {
      lv_label_set_text_fmt(label_time, "%02lu:%02lu", (secs / 60) % 24,
                            secs % 60);
    }
    if (label_date) {
      lv_label_set_text(label_date, "01 Jan");
    }
    lvgl_port_unlock();
  }
}

// ====================================================================================
// MAIN
// ====================================================================================

void app_main(void) {
  ESP_LOGI(TAG, "=========================================");
  ESP_LOGI(TAG, "  Smart Panel 7\" - GUITION JC1060P470C");
  ESP_LOGI(TAG, "  ESP-IDF 6.1 | LVGL 9.4 | 1024x600");
  ESP_LOGI(TAG, "=========================================");

  // Init NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Init WiFi (via ESP32-C6)
  wifi_init();

  // Auto-connect to saved WiFi network if credentials exist
  if (wifi_has_saved_credentials()) {
    char saved_ssid[33] = {0};
    char saved_pass[65] = {0};
    if (wifi_load_credentials(saved_ssid, sizeof(saved_ssid), saved_pass,
                              sizeof(saved_pass)) == ESP_OK) {
      ESP_LOGI(TAG, "Auto-connecting to saved WiFi: %s", saved_ssid);
      wifi_start();
      snprintf(wifi_selected_ssid, sizeof(wifi_selected_ssid), "%s",
               saved_ssid);
      snprintf(wifi_password_input, sizeof(wifi_password_input), "%s",
               saved_pass);
      wifi_connect_to(saved_ssid, saved_pass);
    }
  }

  // Init Bluetooth (via ESP32-C6)
  // Note: Bluetooth runs on the same ESP32-C6 co-processor as WiFi
  if (bluetooth_init() != ESP_OK) {
    ESP_LOGW(TAG, "Bluetooth init failed - BT features will be unavailable");
    bluetooth_enabled = false;
  }

  // NOTE: SD Card disabled - SDMMC controller is used by ESP-Hosted for C6
  // communication Both SDMMC slots are occupied. To use SD card, need SPI-based
  // SD driver. if (sd_card_init() != ESP_OK) {
  //   ESP_LOGW(TAG, "SD Card init failed - storage features limited");
  // }

  // Init hardware
  ESP_ERROR_CHECK(backlight_init());

  esp_lcd_panel_io_handle_t panel_io;
  esp_lcd_panel_handle_t panel_handle;
  ESP_ERROR_CHECK(display_init(&panel_io, &panel_handle));
  touch_init();

  // Init LVGL
  const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
  ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

  const lvgl_port_display_cfg_t disp_cfg = {
      .io_handle = panel_io,
      .panel_handle = panel_handle,
      .buffer_size = LCD_H_RES * 50,
      .double_buffer = true,
      .hres = LCD_H_RES,
      .vres = LCD_V_RES,
      .monochrome = false,
      .color_format = LV_COLOR_FORMAT_RGB565,
      .rotation = {.swap_xy = false, .mirror_x = false, .mirror_y = false},
      .flags = {.buff_dma = true, .buff_spiram = true, .sw_rotate = false},
  };
  const lvgl_port_display_dsi_cfg_t dsi_cfg = {
      .flags = {.avoid_tearing = false}};
  main_display = lvgl_port_add_disp_dsi(&disp_cfg, &dsi_cfg);

  if (touch_handle) {
    const lvgl_port_touch_cfg_t touch_cfg = {.disp = main_display,
                                             .handle = touch_handle};
    lvgl_port_add_touch(&touch_cfg);
  }

  create_ui();
  backlight_set(100);

  ESP_LOGI(TAG, "========================================");
  ESP_LOGI(TAG, "INIT COMPLETE");
  ESP_LOGI(TAG, "========================================");

  while (true) {
    update_status_bar();

#if CONFIG_BT_ENABLED
    // Update BLE scan results UI when scan completes (thread-safe approach)
    if (bt_scan_update_pending) {
      bt_scan_update_pending = false;
      if (lvgl_port_lock(100)) {
        update_bt_list();
        if (bt_status_label) {
          char status[64];
          snprintf(status, sizeof(status), "%d appareils BLE trouves",
                   bt_scan_count);
          lv_label_set_text(bt_status_label, status);
        }
        lvgl_port_unlock();
      }
    }
#endif

    vTaskDelay(pdMS_TO_TICKS(500)); // Check more frequently
  }
}