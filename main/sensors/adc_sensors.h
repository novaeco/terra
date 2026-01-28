#ifndef ADC_SENSORS_H
#define ADC_SENSORS_H

int adc_init(void);
int adc_read_raw(int channel, int *value);
int adc_calibrate(void);
float adc_to_voltage(int raw);

#endif /* ADC_SENSORS_H */