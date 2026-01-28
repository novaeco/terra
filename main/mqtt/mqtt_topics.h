#ifndef MQTT_TOPICS_H
#define MQTT_TOPICS_H

/*
 * Defines for MQTT topics used by the ESP32 Reptile Manager.  In
 * production these would be used to construct topic strings for
 * publishing sensor data, alerts and status messages.
 */

#define MQTT_TOPIC_SENSORS_ALL            "reptile/sensors/all"
#define MQTT_TOPIC_SENSORS_TEMPERATURE    "reptile/sensors/temperature"
#define MQTT_TOPIC_SENSORS_HUMIDITY       "reptile/sensors/humidity"
#define MQTT_TOPIC_ALERTS_TEMP_HIGH       "reptile/alerts/temperature_high"
#define MQTT_TOPIC_ALERTS_TEMP_LOW        "reptile/alerts/temperature_low"
#define MQTT_TOPIC_STATUS_ONLINE          "reptile/status/online"
#define MQTT_TOPIC_STATUS_STATS           "reptile/status/stats"

#endif /* MQTT_TOPICS_H */