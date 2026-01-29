#include <stdio.h>
#include "ota_manager.h"

/*
 * OTA manager implementation.
 *
 * Provides functions to initialise OTA update support and perform
 * firmware updates from a given HTTPS URL.  The
 * esp_https_ota() helper from ESP‑IDF is used to download and
 * install the new firmware.  After a successful OTA the device
 * will reboot automatically.  For simplicity signature
 * verification is assumed to be handled by the OTA API and the
 * server certificate is referenced via the certificates module.
 */

#include "esp_https_ota.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "security/certificates.h"

static const char *TAG_OTA = "ota";

int ota_init(void)
{
    // Nothing to initialise for OTA in this implementation
    return 0;
}

int ota_update_from_url(const char *url)
{
    if (!url) {
        return -1;
    }
    esp_http_client_config_t http_config = {
        .url = url,
        .cert_pem = server_cert_pem_start,
        .timeout_ms = 30000
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &http_config
    };
    ESP_LOGI(TAG_OTA, "Starting OTA from %s", url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG_OTA, "OTA update succeeded, rebooting...");
        esp_restart();
        return 0;
    } else {
        ESP_LOGE(TAG_OTA, "OTA failed: %s", esp_err_to_name(ret));
        return -1;
    }
}
