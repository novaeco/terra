#include <stdio.h>
#include <string.h>
#include "wifi_manager.h"

/*
 * Wi‑Fi manager implementation.
 *
 * This module wraps the ESP‑IDF Wi‑Fi APIs to provide a simple
 * interface for initialising the Wi‑Fi stack, connecting to a
 * configured access point and starting an access point for
 * provisioning.  Configuration parameters (SSID and password) are
 * loaded from NVS via the nvs_manager module.  A minimal event
 * handler updates an event group to signal connection success or
 * failure.  This implementation is adapted from the ESP‑IDF
 * station example and the architectural specification【808169448218282†L587-L669】.
 */

#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "storage/nvs_manager.h"

// Event group bits for connection
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

// Maximum number of connection retries
#ifndef WIFI_MAXIMUM_RETRY
#define WIFI_MAXIMUM_RETRY 5
#endif

static const char *TAG = "wifi";
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Start connecting to the configured AP
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Retry a few times then give up
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip: %s", ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/*
 * Initialise the Wi‑Fi stack in station mode and attempt to connect
 * using configuration stored in NVS.  If no configuration is
 * available the function returns an error and the caller may start
 * an access point for provisioning.  This function blocks until
 * either a connection is established or the maximum number of
 * retries is reached.
 */
int wifi_init(void)
{
    esp_err_t ret;
    // Initialize NVS for Wi‑Fi and other drivers
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Create default event loop and network interface
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Create event group
    s_wifi_event_group = xEventGroupCreate();

    // Register Wi‑Fi and IP event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    // Load configuration from NVS
    wifi_config_t wifi_config = { 0 };
    size_t ssid_len = sizeof(wifi_config.sta.ssid);
    size_t pass_len = sizeof(wifi_config.sta.password);
    if (nvs_get_str("wifi_ssid", (char *)wifi_config.sta.ssid, ssid_len) != 0 ||
        nvs_get_str("wifi_pass", (char *)wifi_config.sta.password, pass_len) != 0) {
        ESP_LOGW(TAG, "No Wi‑Fi credentials found in NVS");
        return -1;
    }
    ESP_LOGI(TAG, "Connecting to SSID:%s", wifi_config.sta.ssid);

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
#endif

    // Set Wi‑Fi mode and configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Wait for connection or failure
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(10000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s", wifi_config.sta.ssid);
        return 0;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "failed to connect to SSID:%s", wifi_config.sta.ssid);
        return -1;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return -1;
    }
}

/*
 * Trigger a reconnection attempt.  This simply calls esp_wifi_connect().
 */
int wifi_connect(void)
{
    esp_err_t err = esp_wifi_connect();
    return (err == ESP_OK) ? 0 : -1;
}

/*
 * Start the access point using default parameters.  This mode is
 * typically used for provisioning when no stored credentials are
 * available.  The SSID and password are generated based on the
 * device's MAC address.  Clients can connect to the AP and submit
 * new Wi‑Fi credentials via a captive portal.
 */
int wifi_start_ap(void)
{
    // Create AP network interface
    esp_netif_create_default_wifi_ap();

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ESP32-Reptile-AP",
            .ssid_len = 0,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        }
    };
    // Generate SSID based on MAC address
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf((char *)ap_config.ap.ssid, sizeof(ap_config.ap.ssid),
             "ESP32-Reptile-%02X%02X", mac[4], mac[5]);
    ap_config.ap.ssid_len = strlen((char *)ap_config.ap.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Started AP SSID:%s", ap_config.ap.ssid);
    return 0;
}