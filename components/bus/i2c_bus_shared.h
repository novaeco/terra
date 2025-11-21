#pragma once

#include "driver/i2c.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the shared I2C0 bus used by onboard peripherals.
 *
 * Idempotent: returns ESP_OK if the bus was already configured.
 */
esp_err_t i2c_bus_shared_init(void);

/**
 * @brief Get the shared I2C port number (I2C_NUM_0).
 */
i2c_port_t i2c_bus_shared_port(void);

/**
 * @brief Default transaction timeout in RTOS ticks for the shared bus.
 */
TickType_t i2c_bus_shared_timeout_ticks(void);

#ifdef __cplusplus
}
#endif

