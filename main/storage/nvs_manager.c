#include <stdio.h>
#include <string.h>
#include "nvs_manager.h"

/*
 * NVS manager implementation.
 *
 * This module wraps the ESP‑IDF NVS APIs to provide simple string
 * get/set functions.  The key/value pairs are stored in the
 * namespace "storage".  The nvs_init() function must be called
 * before any get or set operations.  Error handling is minimal;
 * functions return 0 on success and -1 on failure.
 */

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG_NVS = "nvs";
static nvs_handle_t s_nvs_handle = 0;

int nvs_init(void)
{
    // Initialize NVS flash (if not done by wifi_manager)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS, "NVS flash init failed: %s", esp_err_to_name(err));
        return -1;
    }
    // Open namespace for read/write
    err = nvs_open("storage", NVS_READWRITE, &s_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return -1;
    }
    return 0;
}

int nvs_get_str(const char *key, char *value, size_t max_len)
{
    if (!key || !value || max_len == 0) {
        return -1;
    }
    size_t required = max_len;
    esp_err_t err = nvs_get_str(s_nvs_handle, key, value, &required);
    if (err == ESP_OK) {
        return 0;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        value[0] = '\0';
        return -1;
    }
    ESP_LOGE(TAG_NVS, "nvs_get_str %s failed: %s", key, esp_err_to_name(err));
    return -1;
}

int nvs_set_str(const char *key, const char *value)
{
    if (!key || !value) {
        return -1;
    }
    esp_err_t err = nvs_set_str(s_nvs_handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS, "nvs_set_str %s failed: %s", key, esp_err_to_name(err));
        return -1;
    }
    err = nvs_commit(s_nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS, "nvs_commit failed: %s", esp_err_to_name(err));
        return -1;
    }
    return 0;
}