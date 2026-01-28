#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * Global configuration macros for the ESP32 Reptile Manager skeleton.
 * These definitions illustrate how a central header can be used to
 * configure various modules at compile time.  Adjust values as
 * required for your application.  In a production build these
 * settings might be generated from menuconfig or a JSON config file.
 */

#define APP_NAME "ESP32 Reptile Manager"
#define APP_VERSION "0.1.0-skeleton"

/* Wi‑Fi credentials (overridden by provisioning at runtime). */
#define DEFAULT_WIFI_SSID     ""
#define DEFAULT_WIFI_PASSWORD ""

/* MQTT broker configuration. */
#define MQTT_BROKER_URI "mqtt://broker.hivemq.com"

#endif /* APP_CONFIG_H */