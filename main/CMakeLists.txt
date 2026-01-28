idf_component_register(SRCS \
    "main.c" \
    "app_config.h" \
    # Wi‑Fi module
    "wifi/wifi_manager.c" \
    "wifi/wifi_provisioning.c" \
    # HTTP server
    "http/http_server.c" \
    "http/websocket.c" \
    "http/routes/api_animals.c" \
    "http/routes/api_regulations.c" \
    "http/routes/api_breeding.c" \
    "http/routes/api_documents.c" \
    "http/routes/api_system.c" \
    # Database
    "database/db_manager.c" \
    "database/db_animals.c" \
    "database/db_regulations.c" \
    "database/db_breeding.c" \
    # Storage
    "storage/storage_manager.c" \
    "storage/nvs_manager.c" \
    "storage/file_manager.c" \
    # Sensors
    "sensors/sensor_manager.c" \
    "sensors/dht22.c" \
    "sensors/ds18b20.c" \
    "sensors/adc_sensors.c" \
    # MQTT
    "mqtt/mqtt_client.c" \
    "mqtt/mqtt_topics.c" \
    # BLE
    "ble/ble_server.c" \
    "ble/ble_services.c" \
    # Security
    "security/auth.c" \
    "security/crypto.c" \
    "security/certificates.c" \
    # OTA
    "ota/ota_manager.c" \
    "ota/rollback.c" \
    # Utils
    "utils/json_utils.c" \
    "utils/uuid.c" \
    "utils/datetime.c" \
    "utils/logger.c" \
)

idf_component_register(INCLUDE_DIRS \
    "." \
    "wifi" \
    "http" \
    "http/routes" \
    "database" \
    "storage" \
    "sensors" \
    "mqtt" \
    "ble" \
    "security" \
    "ota" \
    "utils"
)