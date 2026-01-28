#include "ds18b20.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "onewire.h"  // Assumed OneWire driver API

/*
 * Implementation of the DS18B20 driver functions.  The code is
 * adapted from the architecture specification provided by the user.
 */

// Maximum number of DS18B20 sensors on the bus; adjust as needed.
#ifndef DS18B20_MAX_SENSORS
#define DS18B20_MAX_SENSORS 8
#endif

esp_err_t ds18b20_init(void)
{
    // No initialisation required for the simple OneWire driver
    return ESP_OK;
}

esp_err_t ds18b20_scan_bus(uint8_t *addresses[], uint8_t *count)
{
    onewire_search_t search;
    uint8_t found = 0;

    onewire_search_start(&search);
    while (onewire_search_next(&search, addresses[found])) {
        // Vérifier family code DS18B20 (0x28)
        if (addresses[found][0] == 0x28) {
            found++;
            if (found >= DS18B20_MAX_SENSORS) break;
        }
    }
    *count = found;
    return ESP_OK;
}

esp_err_t ds18b20_read_temp(uint8_t *address, float *temp)
{
    uint8_t scratchpad[9];

    // 1. Convert T command
    onewire_reset();
    onewire_select(address);
    onewire_write_byte(0x44);  // CONVERT_T
    vTaskDelay(pdMS_TO_TICKS(750));  // Wait conversion

    // 2. Read scratchpad
    onewire_reset();
    onewire_select(address);
    onewire_write_byte(0xBE);  // READ_SCRATCHPAD
    onewire_read_bytes(scratchpad, 9);

    // 3. Vérifier CRC
    if (!onewire_crc8(scratchpad, 8) == scratchpad[8]) {
        return ESP_ERR_INVALID_CRC;
    }

    // 4. Convertir
    int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
    *temp = (float)raw / 16.0f;

    return ESP_OK;
}