#ifndef DHT22_H
#define DHT22_H

int dht22_init(void);
int dht22_read(float *temperature, float *humidity);

#endif /* DHT22_H */