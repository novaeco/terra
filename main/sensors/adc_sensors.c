#include <stdio.h>
#include "adc_sensors.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

/*
 * ADC sensors implementation.
 *
 * This module configures the ADC1 peripheral for 12‑bit reads and
 * provides functions to read raw values and convert them to a
 * voltage.  Calibration support is omitted for brevity; refer to
 * the ESP‑IDF ADC calibration example for full calibration.
 */

static adc_oneshot_unit_handle_t s_adc_handle;

int adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    esp_err_t err = adc_oneshot_new_unit(&init_config, &s_adc_handle);
    return (err == ESP_OK) ? 0 : -1;
}

int adc_read_raw(int channel, int *value)
{
    if (!value) {
        return -1;
    }
    if (!s_adc_handle) {
        return -1;
    }
    adc_channel_t ch = (adc_channel_t)channel;
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12
    };
    if (adc_oneshot_config_channel(s_adc_handle, ch, &chan_cfg) != ESP_OK) {
        return -1;
    }
    if (adc_oneshot_read(s_adc_handle, ch, value) != ESP_OK) {
        return -1;
    }
    return 0;
}

int adc_calibrate(void)
{
    // Calibration is not implemented; return success
    return 0;
}

float adc_to_voltage(int raw)
{
    // Convert raw reading to voltage (approximate), assuming 12‑bit and 3.3V
    return (float)raw * 3.3f / 4095.0f;
}
