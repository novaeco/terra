#include <stdio.h>
#include "dht22.h"
#include "esp_random.h"
#include "esp_system.h"

/*
 * Simple DHT22 driver.
 *
 * For demonstration purposes this implementation returns pseudo‑random
 * temperature and humidity values.  A real implementation would
 * perform the DHT22 start signal handshake and bit‑bang the GPIO
 * lines to read the sensor data.  See the ESP‑IDF example
 * "dht" component for a production ready driver.
 */

int dht22_init(void)
{
    // Nothing to initialise for the simple random generator
    return 0;
}

int dht22_read(float *temperature, float *humidity)
{
    if (!temperature || !humidity) {
        return -1;
    }
    // Generate deterministic pseudo‑random values in a reasonable range
    uint32_t r = esp_random();
    *temperature = 20.0f + (float)(r % 1000) / 100.0f; // 20.0 – 29.9°C
    *humidity    = 40.0f + (float)((r >> 10) % 1000) / 100.0f; // 40 – 49.9 %RH
    return 0;
}
