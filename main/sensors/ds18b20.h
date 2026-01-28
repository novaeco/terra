#ifndef DS18B20_H
#define DS18B20_H

/*
 * DS18B20 temperature sensor driver.
 *
 * This header declares functions to scan a OneWire bus for DS18B20
 * sensors and read temperatures.  The implementation is based on
 * example code from the project specification.  It assumes that a
 * OneWire stack is available via the `onewire_*` functions.
 */

#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the DS18B20 driver.  In this example there is no
 * specific initialisation required, so the function simply returns
 * ESP_OK.  Provided for API completeness.
 */
esp_err_t ds18b20_init(void);

/**
 * Scan the OneWire bus and return the addresses of all DS18B20
 * sensors found.  The caller must provide an array of pointers and
 * a pointer to a count variable.  The maximum number of sensors
 * detected is limited by DS18B20_MAX_SENSORS (defined elsewhere).
 *
 * @param addresses Array of pointers to store sensor addresses
 * @param count Pointer to store number of sensors found
 * @return ESP_OK on success, or an error code
 */
esp_err_t ds18b20_scan_bus(uint8_t *addresses[], uint8_t *count);

/**
 * Read the temperature from a DS18B20 sensor.
 *
 * @param address 8‑byte OneWire address of the sensor
 * @param temp Pointer to float where the temperature (°C) will be stored
 * @return ESP_OK on success, or an error code
 */
esp_err_t ds18b20_read_temp(uint8_t *address, float *temp);

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_H */