#include <stdio.h>
#include "adc_sensors.h"
#include "driver/adc.h"
#include "esp_system.h"

/*
 * ADC sensors implementation.
 *
 * This module configures the ADC1 peripheral for 12‑bit reads and
 * provides functions to read raw values and convert them to a
 * voltage.  Calibration support is omitted for brevity; refer to
 * the ESP‑IDF ADC calibration example for full calibration.
 */

int adc_init(void)
{
    // Configure width to 12 bits for all channels
    adc1_config_width(ADC_WIDTH_BIT_12);
    return 0;
}

int adc_read_raw(int channel, int *value)
{
    if (!value) {
        return -1;
    }
    // Configure attenuation for the specific channel (11 dB for 0-3.3V)
    adc1_channel_t ch = (adc1_channel_t)channel;
    adc1_config_channel_atten(ch, ADC_ATTEN_DB_11);
    int raw = adc1_get_raw(ch);
    if (raw < 0) {
        return -1;
    }
    *value = raw;
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