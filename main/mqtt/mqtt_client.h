#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

/*
 * Initialise the MQTT client.
 *
 * Reads broker URI from NVS (key "mqtt_uri").  If none is
 * available the client will connect to a default public broker.
 * Registers an event handler to log connection and message events.
 */
int mqtt_client_init(void);

/*
 * Publish a message to the given topic.  Returns 0 on success.
 */
int mqtt_client_publish(const char *topic, const char *payload);

/*
 * Subscribe to the given topic.  Returns 0 on success.
 */
int mqtt_client_subscribe(const char *topic);

#endif /* MQTT_CLIENT_H */