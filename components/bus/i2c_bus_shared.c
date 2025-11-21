#include "i2c_bus_shared.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_check.h"
#include "esp_log.h"

#define SHARED_I2C_PORT         I2C_NUM_0
#define SHARED_I2C_SDA_GPIO     GPIO_NUM_8
#define SHARED_I2C_SCL_GPIO     GPIO_NUM_9
#define SHARED_I2C_FREQ_HZ      400000
#define SHARED_I2C_TIMEOUT_MS   50

static const char *TAG = "i2c_bus_shared";
static bool s_initialized = false;

esp_err_t i2c_bus_shared_init(void)
{
    if (s_initialized)
    {
        return ESP_OK;
    }

    const i2c_config_t cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SHARED_I2C_SDA_GPIO,
        .scl_io_num = SHARED_I2C_SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = SHARED_I2C_FREQ_HZ,
        .clk_flags = 0,
    };

    esp_err_t err = i2c_param_config(SHARED_I2C_PORT, &cfg);
    if ((err != ESP_OK) && (err != ESP_ERR_INVALID_STATE))
    {
        ESP_LOGE(TAG, "i2c_param_config failed (%s)", esp_err_to_name(err));
        return err;
    }

    err = i2c_driver_install(SHARED_I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
    if ((err != ESP_OK) && (err != ESP_ERR_INVALID_STATE))
    {
        ESP_LOGE(TAG, "i2c_driver_install failed (%s)", esp_err_to_name(err));
        return err;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Shared I2C initialized on port %d (SDA=%d SCL=%d %u Hz)",
             SHARED_I2C_PORT,
             SHARED_I2C_SDA_GPIO,
             SHARED_I2C_SCL_GPIO,
             SHARED_I2C_FREQ_HZ);

    return ESP_OK;
}

i2c_port_t i2c_bus_shared_port(void)
{
    return SHARED_I2C_PORT;
}

TickType_t i2c_bus_shared_timeout_ticks(void)
{
    return pdMS_TO_TICKS(SHARED_I2C_TIMEOUT_MS);
}

