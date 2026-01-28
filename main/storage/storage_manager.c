#include <stdio.h>
#include "storage_manager.h"

/*
 * Storage manager implementation for SPIFFS.
 *
 * Mounts and unmounts the SPIFFS filesystem using the ESP‑IDF
 * VFS API.  On mount the filesystem will be formatted if it has
 * never been initialised.  Paths below "/spiffs" will be available
 * via standard C file I/O after a successful mount.
 */

#include "esp_spiffs.h"
#include "esp_log.h"

static const char *TAG_STORAGE = "storage";

int storage_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 8,
        .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_STORAGE, "Failed to mount SPIFFS: %s", esp_err_to_name(ret));
        return -1;
    }
    size_t total = 0, used = 0;
    esp_spiffs_info(conf.partition_label, &total, &used);
    ESP_LOGI(TAG_STORAGE, "SPIFFS mounted, total=%d bytes, used=%d bytes", (int)total, (int)used);
    return 0;
}

int storage_unmount(void)
{
    esp_err_t ret = esp_vfs_spiffs_unregister(NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_STORAGE, "Failed to unmount SPIFFS: %s", esp_err_to_name(ret));
        return -1;
    }
    return 0;
}