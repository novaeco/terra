#include <stdio.h>
#include <string.h>
#include "sensor_manager.h"
#include "dht22.h"
#include "ds18b20.h"
#include "mqtt/mqtt_client.h"
#include "mqtt/mqtt_topics.h"
#include "utils/logger.h"

/*
 * Sensor manager implementation.
 *
 * This module initialises supported sensors and provides a simple
 * function to read all sensors.  DHT22 readings are simulated via
 * pseudo‑random numbers; DS18B20 sensors are scanned on the
 * OneWire bus.  Readings are logged and published via MQTT.
 */

// Maximum DS18B20 sensors supported
#ifndef DS18B20_MAX_SENSORS
#define DS18B20_MAX_SENSORS 8
#endif

static uint8_t s_ds_addresses[DS18B20_MAX_SENSORS][8];
static uint8_t s_ds_count = 0;

int sensors_init(void)
{
    // Initialise DHT22 (pseudo‑random generator)
    dht22_init();

    // Initialise DS18B20 driver and scan for devices
    ds18b20_init();
    uint8_t *addr_ptrs[DS18B20_MAX_SENSORS];
    for (int i = 0; i < DS18B20_MAX_SENSORS; i++) {
        addr_ptrs[i] = s_ds_addresses[i];
    }
    if (ds18b20_scan_bus(addr_ptrs, &s_ds_count) != ESP_OK) {
        log_warn("sensors", "DS18B20 scan failed");
    } else {
        log_info("sensors", "Found %d DS18B20 sensors", s_ds_count);
    }
    return 0;
}

int sensors_read(void)
{
    // Read DHT22
    float temp = 0.0f, hum = 0.0f;
    if (dht22_read(&temp, &hum) == 0) {
        log_info("sensors", "DHT22: T=%.2f°C H=%.2f%%", temp, hum);
    } else {
        log_warn("sensors", "Failed to read DHT22");
    }

    // Read DS18B20 sensors
    for (uint8_t i = 0; i < s_ds_count; i++) {
        float t = 0.0f;
        if (ds18b20_read_temp(s_ds_addresses[i], &t) == ESP_OK) {
            log_info("sensors", "DS18B20[%d]: %.2f°C", i, t);
        } else {
            log_warn("sensors", "DS18B20[%d] read failed", i);
        }
    }

    // Optionally publish aggregated sensor data via MQTT
    char payload[128];
    snprintf(payload, sizeof(payload), "{\"temperature\":%.2f,\"humidity\":%.2f}", temp, hum);
    mqtt_client_publish(MQTT_TOPIC_SENSORS_ALL, payload);
    return 0;
}