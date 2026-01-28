#include <stdio.h>
#include "rollback.h"

/*
 * OTA rollback implementation.
 *
 * If the current running firmware is marked as pending verification
 * and diagnostics fail, this function can be called to roll back
 * to the previous partition.  It uses the ESP‑IDF OTA API.
 */

#include "esp_ota_ops.h"
#include "esp_log.h"

int ota_rollback(void)
{
    // Mark the current app as invalid and reboot into the previous one
    esp_err_t err = esp_ota_mark_app_invalid_rollback_and_reboot();
    if (err != ESP_OK) {
        ESP_LOGE("rollback", "Failed to rollback: %s", esp_err_to_name(err));
        return -1;
    }
    return 0;
}