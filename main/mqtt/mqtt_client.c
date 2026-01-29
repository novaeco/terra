#include <stdio.h>
#include <string.h>
#include <mqtt_client.h>

/*
 * MQTT client implementation.
 *
 * This module uses the ESP‑IDF MQTT client to connect to a broker,
 * publish sensor data and subscribe to command topics.  Broker
 * parameters are loaded from NVS; if none are present a default
 * public broker is used.  The event handler logs connection
 * events and prints received messages.  See the ESP‑IDF MQTT
 * example for more details.
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "storage/nvs_manager.h"

static const char *TAG_MQTT = "mqtt";
static esp_mqtt_client_handle_t s_mqtt_client = NULL;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT connected");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG_MQTT, "MQTT disconnected");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQTT, "Received on %.*s: %.*s",
                 event->topic_len, event->topic,
                 event->data_len, event->data);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    mqtt_event_handler_cb(event_data);
}

int mqtt_client_init(void)
{
    // If client already initialised, do nothing
    if (s_mqtt_client) {
        return 0;
    }
    char broker_uri[128] = {0};
    // Read broker URI from NVS; fallback to test broker
    if (nvsman_get_str("mqtt_uri", broker_uri, sizeof(broker_uri)) != 0) {
        strncpy(broker_uri, "mqtt://broker.hivemq.com", sizeof(broker_uri) - 1);
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_uri,
    };
    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (!s_mqtt_client) {
        ESP_LOGE(TAG_MQTT, "Failed to create MQTT client");
        return -1;
    }
    esp_mqtt_client_register_event(s_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_err_t err = esp_mqtt_client_start(s_mqtt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_MQTT, "Failed to start MQTT client: %s", esp_err_to_name(err));
        return -1;
    }
    ESP_LOGI(TAG_MQTT, "MQTT client started with URI %s", broker_uri);
    return 0;
}

int mqtt_client_publish(const char *topic, const char *payload)
{
    if (!s_mqtt_client) {
        ESP_LOGW(TAG_MQTT, "MQTT client not initialised");
        return -1;
    }
    int msg_id = esp_mqtt_client_publish(s_mqtt_client, topic,
                                         payload ? payload : "",
                                         0, /* length: 0 = strlen */
                                         1, /* QoS 1 */
                                         0  /* no retain */);
    return (msg_id >= 0) ? 0 : -1;
}

int mqtt_client_subscribe(const char *topic)
{
    if (!s_mqtt_client) {
        ESP_LOGW(TAG_MQTT, "MQTT client not initialised");
        return -1;
    }
    int msg_id = esp_mqtt_client_subscribe(s_mqtt_client, topic, 1);
    return (msg_id >= 0) ? 0 : -1;
}
