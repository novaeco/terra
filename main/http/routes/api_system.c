#include <stdio.h>
#include "api_system.h"

/*
 * System API implementation.
 *
 * These functions provide basic system information, reboot
 * functionality and log retrieval.  The current implementation
 * prints JSON to stdout and invokes esp_restart() for reboot.  In a
 * real HTTP server these functions would write to the httpd
 * response rather than printing to the console.
 */

#include "esp_system.h"
#include "esp_timer.h"
#include "cJSON.h"

int api_system_get_stats(void)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "uptime", (double)esp_timer_get_time() / 1e6);
    cJSON_AddNumberToObject(root, "heap_free", (double)esp_get_free_heap_size());
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (json_str) {
        printf("%s\n", json_str);
        free(json_str);
    }
    return 0;
}

int api_system_reboot(void)
{
    // Print a message and reboot the device
    printf("[api/system] rebooting device...\n");
    esp_restart();
    return 0;
}

int api_system_get_logs(void)
{
    // Log retrieval not implemented; print placeholder
    printf("[api/system] logs: (not implemented)\n");
    return 0;
}