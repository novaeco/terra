#include <stdio.h>
#include "app_config.h"
#include "wifi/wifi_manager.h"
#include "http/http_server.h"
#include "database/db_manager.h"
#include "sensors/sensor_manager.h"
#include "mqtt/mqtt_client.h"
#include "security/auth.h"
#include "ota/ota_manager.h"
#include "storage/storage_manager.h"
#include "storage/nvs_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*
 * Entry point for the ESP32 Reptile Manager skeleton.  A real
 * implementation would initialize hardware, start FreeRTOS tasks and
 * configure all modules.  Here we simply print the application
 * information and call a few stub initializers to illustrate the
 * expected control flow.
 */
void app_main(void)
{
    printf("\n============================================\n");
    printf("  %s\n", APP_NAME);
    printf("  Version: %s\n", APP_VERSION);
    printf("============================================\n\n");

    // Initialise modules.  Each function currently returns 0 and
    // prints a message; replace with real implementations.
    // Initialise storage and NVS before any other operations
    storage_mount();
    nvs_init();

    // Initialise Wi‑Fi; if credentials are missing start AP for provisioning
    if (wifi_init() != 0) {
        // Start AP mode for provisioning
        wifi_start_ap();
    }
    // Initialise database
    db_init();
    // Initialise sensors
#if APP_SENSORS_ENABLED
    sensors_init();
#else
    printf("Sensors disabled via APP_SENSORS_ENABLED=0\n");
#endif
    // Start MQTT client
    mqtt_client_init();
    // Start HTTP server
    http_server_start();
    // Initialise OTA support
    ota_init();

    // Simple periodic sensor reading loop (non‑blocking tasks omitted)
#if APP_SENSORS_ENABLED
    while (1) {
        sensors_read();
        vTaskDelay(pdMS_TO_TICKS(60000)); // 1 minute
    }
#endif

    // Unreachable; in practice the FreeRTOS scheduler runs tasks
    // and the loop above yields to other tasks.
    // storage_unmount();
    // printf("Initialisation complete.\n");
}
