# Architecture ESP32-S3 Reptile Manager Server

## 📐 Vue d'ensemble de l'architecture

### Modèle Multi-Couches

```
┌────────────────────────────────────────────────────────────┐
│                    APPLICATIONS CLIENTS                    │
│  Web Browser │ Mobile App │ Desktop App │ API Clients     │
└─────────────────────┬──────────────────────────────────────┘
                      │
            ┌─────────┴─────────┐
            │                   │
┌───────────▼─────────┐ ┌──────▼────────────┐
│   HTTP/HTTPS REST   │ │   WebSocket       │
│   API Endpoints     │ │   Real-time       │
└──────────┬──────────┘ └───────┬───────────┘
           │                    │
           │  ┌─────────────────┼─────────┐
           │  │                 │         │
┌──────────▼──▼─────────────────▼─────────▼──────────┐
│            ESP32-S3 FIRMWARE LAYERS                 │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌─────────────────────────────────────────────┐  │
│  │        APPLICATION LAYER                    │  │
│  │  Business Logic │ Routing │ Controllers     │  │
│  └─────────────┬───────────────────────────────┘  │
│                │                                   │
│  ┌─────────────▼───────────────────────────────┐  │
│  │        SERVICE LAYER                        │  │
│  │  DB Mgr │ Sensor Mgr │ Auth │ OTA          │  │
│  └─────────────┬───────────────────────────────┘  │
│                │                                   │
│  ┌─────────────▼───────────────────────────────┐  │
│  │        INFRASTRUCTURE LAYER                 │  │
│  │  HTTP │ MQTT │ BLE │ Storage │ Crypto       │  │
│  └─────────────┬───────────────────────────────┘  │
│                │                                   │
│  ┌─────────────▼───────────────────────────────┐  │
│  │        HARDWARE ABSTRACTION LAYER (HAL)     │  │
│  │  Wi-Fi │ I2C │ SPI │ GPIO │ ADC │ NVS       │  │
│  └─────────────┬───────────────────────────────┘  │
│                │                                   │
└────────────────┼───────────────────────────────────┘
                 │
┌────────────────▼───────────────────────────────────┐
│               ESP-IDF v6.1 FRAMEWORK               │
│  FreeRTOS │ lwIP │ mbedTLS │ Drivers               │
└────────────────┬───────────────────────────────────┘
                 │
┌────────────────▼───────────────────────────────────┐
│        HARDWARE: ESP32-S3-DevKitC-1-N32R16V        │
│  Dual Xtensa LX7 @ 240MHz │ 32MB Flash │ 16MB PSRAM│
└────────────────────────────────────────────────────┘
```

---

## 🧩 Architecture Modulaire Détaillée

### 1. Module Wi-Fi & Réseau

**Responsabilités:**
- Gestion connexion Wi-Fi (Station + AP)
- Reconnexion automatique intelligente
- Provisioning en mode AP
- Suivi RSSI et qualité réseau
- Configuration persistante (NVS)

**Composants:**
```
wifi/
├── wifi_manager.c/h         # Gestionnaire principal
│   ├── wifi_init()          # Init stack Wi-Fi
│   ├── wifi_connect()       # Connexion STA
│   ├── wifi_start_ap()      # Démarrage AP
│   ├── wifi_event_handler() # Gestion événements
│   └── wifi_save_config()   # Persistance NVS
│
└── wifi_provisioning.c/h    # Provisioning captif portal
    ├── prov_start()         # Démarre serveur provisioning
    ├── prov_handle_web()    # Interface web configuration
    └── prov_complete()      # Finalise configuration
```

**Event Flow:**
```
Boot → Check NVS Config
  ├─ Config found → Try STA connection
  │   ├─ Success → CONNECTED state
  │   └─ Fail → Fallback AP mode
  └─ No config → Start AP for provisioning
```

**États:**
```c
DISCONNECTED → CONNECTING → CONNECTED
                    ↓            ↓
                  FAILED → AP_STARTED
```

---

### 2. Module HTTP Server

**Responsabilités:**
- Serveur HTTP/HTTPS multithread
- API REST complète
- WebSocket pour le temps réel
- Authentification JWT
- Rate limiting

**Architecture:**
```
http/
├── http_server.c/h          # Serveur HTTP principal
│   ├── server_start()       # Démarre serveur
│   ├── server_register()    # Enregistre routes
│   ├── server_middleware()  # Auth, CORS, etc.
│   └── server_error()       # Gestion erreurs
│
├── websocket.c/h            # WebSocket handler
│   ├── ws_handle_connect()  # Nouvelle connexion
│   ├── ws_broadcast()       # Broadcast message
│   └── ws_handle_frame()    # Traite frames
│
└── routes/
    ├── api_animals.c/h      # CRUD animaux
    │   ├── GET /api/v1/animals
    │   ├── POST /api/v1/animals
    │   ├── GET /api/v1/animals/{id}
    │   ├── PUT /api/v1/animals/{id}
    │   └── DELETE /api/v1/animals/{id}
    │
    ├── api_regulations.c/h  # Réglementation
    │   ├── GET /api/v1/regulations/species/{name}
    │   ├── GET /api/v1/regulations/animals/{id}/status
    │   └── GET /api/v1/regulations/alerts
    │
    ├── api_breeding.c/h     # Reproduction
    │   ├── GET /api/v1/breeding/cycles
    │   ├── POST /api/v1/breeding/cycles
    │   └── GET /api/v1/breeding/cycles/{id}
    │
    ├── api_documents.c/h    # Documents
    │   ├── POST /api/v1/documents/generate
    │   └── GET /api/v1/documents/{id}/download
    │
    └── api_system.c/h       # Système
        ├── GET /api/v1/system/stats
        ├── POST /api/v1/system/reboot
        └── GET /api/v1/system/logs
```

**Request Flow:**
```
Client Request
    ↓
[HTTP Server Task]
    ↓
[Middleware Chain]
    ├─ Auth JWT
    ├─ Rate Limit
    ├─ CORS
    └─ Content-Type
    ↓
[Route Handler]
    ↓
[Service Layer]
    ↓
[Database/Storage]
    ↓
[Response JSON]
    ↓
Client Response
```

**Middleware Pattern:**
```c
typedef esp_err_t (*http_middleware_t)(httpd_req_t *req, void *ctx);

// Chaîne de middleware
middleware_chain[] = {
    auth_middleware,
    rate_limit_middleware,
    cors_middleware,
    logging_middleware,
    NULL
};
```

---

### 3. Module Database (SQLite)

**Responsabilités:**
- Gestion base SQLite embarquée
- CRUD opérations
- Transactions ACID
- Cache PSRAM
- Migrations

**Architecture:**
```
database/
├── db_manager.c/h           # Gestionnaire DB
│   ├── db_init()            # Init + migrations
│   ├── db_open()            # Connexion pool
│   ├── db_execute()         # Exécute SQL
│   ├── db_transaction()     # Transactions
│   └── db_backup()          # Backup vers SPIFFS
│
├── db_animals.c/h           # Requêtes animaux
│   ├── db_animal_create()
│   ├── db_animal_get()
│   ├── db_animal_update()
│   ├── db_animal_delete()
│   └── db_animal_search()
│
├── db_regulations.c/h       # Requêtes réglementation
│   ├── db_species_get_regulation()
│   ├── db_compliance_check()
│   └── db_alerts_get_active()
│
├── db_breeding.c/h          # Requêtes reproduction
│   ├── db_cycle_create()
│   ├── db_cycle_get()
│   ├── db_offspring_add()
│   └── db_genealogy_get()
│
└── migrations/              # Scripts migration
    ├── 001_initial_schema.sql
    ├── 002_add_sensors.sql
    └── 003_add_indexes.sql
```

**Schéma Core:**
```sql
-- Tables principales (version simplifiée)
CREATE TABLE animals (
    id TEXT PRIMARY KEY,
    species_name TEXT NOT NULL,
    common_name TEXT,
    sex TEXT CHECK(sex IN ('M','F','UNKNOWN')),
    date_birth INTEGER,
    date_acquisition INTEGER NOT NULL,
    status TEXT CHECK(status IN ('ACTIVE','SOLD','DECEASED','TRANSFERRED')),
    provenance_type TEXT,
    provenance_vendor TEXT,
    metadata_json TEXT,  -- JSON pour données flexibles
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL,
    FOREIGN KEY (species_name) REFERENCES species_regulations(scientific_name)
);

CREATE TABLE species_regulations (
    scientific_name TEXT PRIMARY KEY,
    common_names TEXT,  -- JSON array
    family TEXT,
    domestic INTEGER NOT NULL DEFAULT 0,
    category TEXT,
    cites_appendix TEXT CHECK(cites_appendix IN ('I','II','III',NULL)),
    eu_annex TEXT CHECK(eu_annex IN ('A','B','C','D',NULL)),
    france_column TEXT CHECK(france_column IN ('a','b','c',NULL)),
    dangerous INTEGER DEFAULT 0,
    invasive INTEGER DEFAULT 0,
    last_updated INTEGER
);

CREATE TABLE breeding_cycles (
    id TEXT PRIMARY KEY,
    male_id TEXT REFERENCES animals(id),
    female_id TEXT REFERENCES animals(id),
    season INTEGER,
    start_date INTEGER,
    end_date INTEGER,
    status TEXT CHECK(status IN ('PLANNING','ACTIVE','COMPLETED','FAILED')),
    clutch_date INTEGER,
    clutch_eggs_total INTEGER,
    clutch_eggs_viable INTEGER,
    incubation_temp_avg REAL,
    notes TEXT,
    created_at INTEGER NOT NULL
);

CREATE TABLE sensor_readings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_type TEXT NOT NULL,
    sensor_location TEXT,
    temperature REAL,
    humidity REAL,
    timestamp INTEGER NOT NULL
);

-- Index pour performances
CREATE INDEX idx_animals_species ON animals(species_name);
CREATE INDEX idx_animals_status ON animals(status);
CREATE INDEX idx_animals_updated ON animals(updated_at DESC);
CREATE INDEX idx_breeding_season ON breeding_cycles(season DESC);
CREATE INDEX idx_sensor_readings_time ON sensor_readings(timestamp DESC);
CREATE INDEX idx_sensor_readings_type ON sensor_readings(sensor_type, timestamp DESC);
```

**Cache Strategy (PSRAM):**
```c
// L1 Cache: Dernières requêtes
typedef struct {
    char key[64];               // Hash requête SQL
    char *result_json;          // Résultat cache
    uint32_t timestamp;         // TTL
    size_t size;
} cache_entry_t;

#define CACHE_MAX_ENTRIES 100
#define CACHE_TTL_SEC 300       // 5 minutes

cache_entry_t *g_cache;  // Alloué en PSRAM
```

**Transaction Pattern:**
```c
esp_err_t db_animal_create_with_logs(animal_t *animal) {
    DB_BEGIN_TRANSACTION();
    
    // 1. Insérer animal
    CHECK_ERROR_GOTO(db_animal_insert(animal), rollback);
    
    // 2. Logger événement
    CHECK_ERROR_GOTO(db_log_event("ANIMAL_CREATED", animal->id), rollback);
    
    // 3. Vérifier conformité
    CHECK_ERROR_GOTO(db_compliance_update(animal->id), rollback);
    
    DB_COMMIT_TRANSACTION();
    return ESP_OK;
    
rollback:
    DB_ROLLBACK_TRANSACTION();
    return ESP_FAIL;
}
```

---

### 4. Module Sensors (Temps réel)

**Responsabilités:**
- Lecture périodique capteurs
- Conversion données ADC/I2C/OneWire
- Buffer circulaire historique
- Alertes seuils
- Publication MQTT/WebSocket

**Architecture:**
```
sensors/
├── sensor_manager.c/h       # Orchestrateur
│   ├── sensor_init_all()    # Init tous capteurs
│   ├── sensor_read_task()   # Task lecture périodique
│   ├── sensor_get_current() # Valeurs actuelles
│   └── sensor_get_history() # Historique N heures
│
├── dht22.c/h                # DHT22 (Temp/Humidity)
│   ├── dht22_init()
│   ├── dht22_read()
│   └── dht22_convert()
│
├── ds18b20.c/h              # DS18B20 OneWire
│   ├── ds18b20_init()
│   ├── ds18b20_scan_bus()   # Détecte capteurs
│   ├── ds18b20_read_temp()
│   └── ds18b20_read_all()   # Tous capteurs
│
└── adc_sensors.c/h          # Capteurs analogiques
    ├── adc_init()
    ├── adc_read_raw()
    ├── adc_calibrate()
    └── adc_to_voltage()
```

**Task Flow:**
```
[Sensor Read Task] (Core 1, High Priority)
    │
    ├─ Every 1 minute:
    │   ├─ Read DHT22 (GPIO 5)
    │   ├─ Read DS18B20 bus (GPIO 4)
    │   ├─ Read ADC channels
    │   ├─ Store to circular buffer (PSRAM)
    │   ├─ Check thresholds → Alert if needed
    │   ├─ Publish MQTT (if connected)
    │   └─ Broadcast WebSocket
    │
    └─ Every 5 minutes:
        └─ Save batch to SQLite
```

**Circular Buffer (PSRAM):**
```c
// Buffer 7 jours de données (1 mesure/minute)
#define SENSOR_BUFFER_SIZE (7 * 24 * 60)

typedef struct {
    uint32_t timestamp;
    float temperature;
    float humidity;
} sensor_sample_t;

sensor_sample_t *g_sensor_buffer;  // 7*24*60*12bytes ≈ 140KB en PSRAM
uint16_t g_buffer_head = 0;
```

**Driver DS18B20 (OneWire):**
```c
// Découverte automatique capteurs sur le bus
esp_err_t ds18b20_scan_bus(uint8_t *addresses[], uint8_t *count) {
    onewire_search_t search;
    uint8_t found = 0;
    
    onewire_search_start(&search);
    while (onewire_search_next(&search, addresses[found])) {
        // Vérifier family code DS18B20 (0x28)
        if (addresses[found][0] == 0x28) {
            found++;
            if (found >= DS18B20_MAX_SENSORS) break;
        }
    }
    *count = found;
    return ESP_OK;
}

// Lecture température
esp_err_t ds18b20_read_temp(uint8_t *address, float *temp) {
    uint8_t scratchpad[9];
    
    // 1. Convert T command
    onewire_reset();
    onewire_select(address);
    onewire_write_byte(0x44);  // CONVERT_T
    vTaskDelay(pdMS_TO_TICKS(750));  // Wait conversion
    
    // 2. Read scratchpad
    onewire_reset();
    onewire_select(address);
    onewire_write_byte(0xBE);  // READ_SCRATCHPAD
    onewire_read_bytes(scratchpad, 9);
    
    // 3. Vérifier CRC
    if (onewire_crc8(scratchpad, 8) != scratchpad[8]) {
        return ESP_ERR_INVALID_CRC;
    }
    
    // 4. Convertir
    int16_t raw = (scratchpad[1] << 8) | scratchpad[0];
    *temp = (float)raw / 16.0f;
    
    return ESP_OK;
}
```

---

### 5. Module MQTT

**Responsabilités:**
- Client MQTT avec QoS
- Publication données capteurs
- Souscription commandes
- Reconnexion automatique
- Buffer messages offline

**Topics:**
```
reptile/
├── sensors/
│   ├── temperature/terrarium_1
│   ├── humidity/terrarium_1
│   └── all                      # Batch JSON
├── alerts/
│   ├── temperature_high
│   ├── temperature_low
│   └── compliance_issue
├── status/
│   ├── online                   # Last Will
│   └── stats
├── commands/
│   ├── reboot
│   ├── ota_update
│   └── set_relay
└── ota/
    └── firmware                 # Receive firmware
```

**Message Format:**
```json
// reptile/sensors/all (every 1 min)
{
  "device_id": "esp32-abc123",
  "timestamp": 1706469120,
  "sensors": [
    {
      "type": "DHT22",
      "location": "terrarium_1",
      "temperature": 28.5,
      "humidity": 65.2
    },
    {
      "type": "DS18B20",
      "location": "water_bowl",
      "temperature": 24.3
    }
  ]
}

// reptile/alerts/temperature_high
{
  "device_id": "esp32-abc123",
  "timestamp": 1706469120,
  "sensor": "terrarium_1",
  "value": 35.2,
  "threshold": 32.0,
  "severity": "WARNING"
}
```

---

### 6. Module Security

**Responsabilités:**
- Authentification JWT
- Chiffrement données sensibles
- Certificats TLS
- Secure Boot / Flash Encryption
- API keys

**JWT Implementation:**
```c
// Génération token
char* jwt_generate(const char *username, const char *role) {
    cJSON *header = cJSON_CreateObject();
    cJSON_AddStringToObject(header, "alg", "HS256");
    cJSON_AddStringToObject(header, "typ", "JWT");
    
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "sub", username);
    cJSON_AddStringToObject(payload, "role", role);
    cJSON_AddNumberToObject(payload, "iat", time(NULL));
    cJSON_AddNumberToObject(payload, "exp", time(NULL) + JWT_EXPIRY_SEC);
    
    // Base64URL encode
    char *header_b64 = base64url_encode(cJSON_Print(header));
    char *payload_b64 = base64url_encode(cJSON_Print(payload));
    
    // Signature HMAC-SHA256
    char signature_input[512];
    snprintf(signature_input, sizeof(signature_input), 
             "%s.%s", header_b64, payload_b64);
    
    uint8_t signature[32];
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                    JWT_SECRET, strlen(JWT_SECRET),
                    signature_input, strlen(signature_input),
                    signature);
    
    char *signature_b64 = base64url_encode(signature, 32);
    
    // Token final: header.payload.signature
    char *token = malloc(strlen(header_b64) + strlen(payload_b64) + 
                         strlen(signature_b64) + 3);
    sprintf(token, "%s.%s.%s", header_b64, payload_b64, signature_b64);
    
    return token;
}
```

**NVS Encryption:**
```c
// Clé chiffrée stockée dans partition nvs_key
esp_err_t nvs_init_encrypted(void) {
    nvs_sec_cfg_t cfg;
    
    // Lire clé depuis partition
    esp_err_t ret = nvs_flash_read_security_cfg(
        nvs_sec_scheme_hmac_sha256(),
        &cfg
    );
    
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        // Première utilisation: générer clé
        nvs_flash_generate_keys(&cfg);
        nvs_flash_write_security_cfg(&cfg);
    }
    
    // Initialiser avec chiffrement
    return nvs_flash_secure_init(&cfg);
}
```

---

### 7. Module OTA

**Responsabilités:**
- Mise à jour firmware OTA
- Dual partition (ota_0 / ota_1)
- Rollback automatique
- Vérification signature
- Progress feedback

**OTA Flow:**
```
User triggers OTA
    ↓
[Download firmware]
    ├─ HTTP chunked
    ├─ Verify signature
    └─ Write to ota_X partition
    ↓
[Mark as pending]
    ↓
[Reboot]
    ↓
[Bootloader]
    ├─ Check new partition
    ├─ Boot new firmware
    └─ Run diagnostics
    ↓
┌─ Success ─┐         ┌─ Fail ─┐
│ Mark valid│         │Rollback│
│ Continue  │         │ Boot old│
└───────────┘         └────────┘
```

**Implémentation:**
```c
esp_err_t ota_update_from_url(const char *url) {
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = server_cert_pem_start,
        .timeout_ms = 30000,
    };
    
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA success, rebooting...");
        esp_restart();
    }
    return ret;
}

// Dans main.c au boot
void app_main(void) {
    // ...
    
    // Vérifier diagnostic
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    esp_ota_get_state_partition(running, &ota_state);
    
    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
        // Nouveau firmware, faire diagnostics
        if (run_diagnostics() == ESP_OK) {
            ESP_LOGI(TAG, "Diagnostics OK, marking valid");
            esp_ota_mark_app_valid_cancel_rollback();
        } else {
            ESP_LOGE(TAG, "Diagnostics failed, rolling back");
            esp_ota_mark_app_invalid_rollback_and_reboot();
        }
    }
}
```

---

## 🔄 FreeRTOS Task Architecture

### Task Mapping

```
Core 0 (PRO_CPU):                    Core 1 (APP_CPU):
┌───────────────────────┐            ┌───────────────────────┐
│ HTTP Server Task      │            │ Sensor Read Task      │
│ Priority: 5           │            │ Priority: 6 (HIGH)    │
│ Stack: 8KB            │            │ Stack: 3KB            │
│ ▪ Handle requests     │            │ ▪ Read sensors        │
│ ▪ Route to handlers   │            │ ▪ Update buffer       │
│ ▪ Send responses      │            │ ▪ Check thresholds    │
└───────────────────────┘            └───────────────────────┘

┌───────────────────────┐            ┌───────────────────────┐
│ DB Manager Task       │            │ BLE Server Task       │
│ Priority: 4           │            │ Priority: 3           │
│ Stack: 4KB            │            │ Stack: 4KB            │
│ ▪ Execute queries     │            │ ▪ Handle connections  │
│ ▪ Transactions        │            │ ▪ GATT operations     │
│ ▪ Cache management    │            │ ▪ Notify clients      │
└───────────────────────┘            └───────────────────────┘

┌───────────────────────┐
│ MQTT Client Task      │
│ Priority: 3           │
│ Stack: 4KB            │
│ ▪ Maintain connection │
│ ▪ Publish messages    │
│ ▪ Handle subscriptions│
└───────────────────────┘

┌───────────────────────┐
│ OTA Task              │
│ Priority: 2           │
│ Stack: 8KB            │
│ ▪ Download firmware   │
│ ▪ Write partition     │
│ ▪ Verify signature    │
└───────────────────────┘

┌───────────────────────┐            ┌───────────────────────┐
│ System Monitor Task   │            │ Watchdog Task         │
│ Priority: 1 (LOW)     │            │ Priority: 15 (MAX)    │
│ Stack: 3KB            │            │ Stack: 2KB            │
│ ▪ Update stats        │            │ ▪ Feed watchdog       │
│ ▪ Check heap          │            │ ▪ Detect deadlocks    │
│ ▪ Log metrics         │            │ ▪ Emergency reset     │
└───────────────────────┘            └───────────────────────┘
```

### Inter-Task Communication

```c
// Queues
QueueHandle_t sensor_data_queue;    // Sensors → DB
QueueHandle_t http_request_queue;   // HTTP → Handlers
QueueHandle_t mqtt_publish_queue;   // Any → MQTT
QueueHandle_t alert_queue;          // Any → Notification

// Semaphores
SemaphoreHandle_t db_mutex;         // Protège accès DB
SemaphoreHandle_t nvs_mutex;        // Protège accès NVS
SemaphoreHandle_t wifi_event_sem;   // Signale événements Wi-Fi

// Event Groups
EventGroupHandle_t system_events;
    // WIFI_CONNECTED_BIT
    // DB_READY_BIT
    // MQTT_CONNECTED_BIT
    // etc.
```

---

## 💾 Memory Management

### Allocation Strategy

```
ESP32-S3 Memory Map:
┌────────────────────────────────────┐
│ IRAM (Instruction RAM)             │  ~400KB
│ ▪ Code critique (ISR, drivers)     │
└────────────────────────────────────┘

┌────────────────────────────────────┐
│ DRAM (Data RAM)                    │  ~300KB
│ ▪ .data, .bss, heap                │
│ ▪ Stack tasks                      │
└────────────────────────────────────┘

┌────────────────────────────────────┐
│ PSRAM (External)                   │  16MB
│ ▪ DB cache                         │  1MB
│ ▪ Sensor history buffer            │  200KB
│ ▪ JSON buffers (large responses)   │  500KB
│ ▪ File upload buffers              │  1MB
│ ▪ HTTP response buffers            │  512KB
│ └─ Available for dynamic alloc     │  ~13MB
└────────────────────────────────────┘

┌────────────────────────────────────┐
│ Flash (32MB)                       │
│ ├─ ota_0: 3MB (firmware)           │
│ ├─ ota_1: 3MB (firmware backup)    │
│ ├─ SPIFFS: 9.75MB (data)           │
│ │   ├─ SQLite DB: ~5MB             │
│ │   ├─ Web UI: ~1MB                │
│ │   ├─ Logs: ~2MB                  │
│ │   └─ Certificates: ~100KB        │
│ └─ NVS: 24KB (config)              │
└────────────────────────────────────┘
```

### Heap Allocation Guidelines

```c
// Petites allocations (<16KB) → DRAM
char *small_buffer = malloc(1024);

// Grandes allocations (>16KB) → PSRAM
char *large_buffer = heap_caps_malloc(100000, MALLOC_CAP_SPIRAM);

// Allocations critiques (ISR-safe) → Internal RAM
char *critical = heap_caps_malloc(512, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

// DMA buffers → DMA capable RAM
void *dma_buf = heap_caps_malloc(4096, MALLOC_CAP_DMA);
```

---

## 📊 Performance Optimization

### 1. Database Optimizations

```sql
-- Indexes sur colonnes fréquemment requêtées
CREATE INDEX idx_animals_species ON animals(species_name);
CREATE INDEX idx_sensor_time ON sensor_readings(timestamp DESC);

-- PRAGMA optimizations
PRAGMA journal_mode = WAL;           -- Write-Ahead Logging
PRAGMA synchronous = NORMAL;         -- Balance perfs/durabilité
PRAGMA cache_size = -256000;         -- 256MB cache (PSRAM)
PRAGMA temp_store = MEMORY;          -- Temp tables en RAM
PRAGMA mmap_size = 268435456;        -- 256MB mmap
```

### 2. HTTP Optimizations

```c
// Pooling connexions
#define HTTP_MAX_SOCKETS 10
#define HTTP_KEEPALIVE_TIMEOUT 30

// Compression réponses
httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

// Caching headers
httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=3600");

// Chunked transfer
httpd_resp_set_type(req, "application/json");
httpd_resp_set_hdr(req, "Transfer-Encoding", "chunked");
```

### 3. Sensor Read Optimizations

- Regrouper les lectures I2C en mode « burst » quand le capteur le permet.
- Limiter les réveils CPU en alignant les lectures sur un cadenceur commun.
- Utiliser des buffers DMA pour les transferts I2C/SPI afin d'économiser du temps CPU.

---

## 🔒 Security Best Practices

### 1. Secure Boot

```bash
# Générer clés
espsecure.py generate_signing_key secure_boot_signing_key.pem

# Activer dans sdkconfig
CONFIG_SECURE_BOOT_V2_ENABLED=y
CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES=y

# Build avec signature
idf.py build
espsecure.py sign_data --keyfile secure_boot_signing_key.pem \
    build/reptile-server.bin -o build/reptile-server-signed.bin
```

### 2. Flash Encryption

```bash
# Générer clé
espsecure.py generate_flash_encryption_key flash_encryption_key.bin

# Activer
CONFIG_SECURE_FLASH_ENC_ENABLED=y
CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT=y  # Dev
CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE=y      # Prod
```

### 3. API Security Checklist

- [ ] JWT avec expiration (24h)
- [ ] Refresh tokens (7 jours)
- [ ] Rate limiting (100 req/min)
- [ ] Input validation (toutes routes)
- [ ] SQL injection protection (prepared statements)
- [ ] XSS protection (échappement JSON)
- [ ] CORS configuré
- [ ] HTTPS en production
- [ ] Secrets en NVS chiffré

---

## 🧪 Testing Strategy

### 1. Unit Tests (Host)

```bash
cd components/database
idf.py build test

# Tests SQLite
test_db_init()
test_db_transaction()
test_db_query_performance()
```

### 2. Integration Tests

```python
# tests/integration/test_http_api.py
def test_animal_crud():
    # Create
    resp = requests.post("http://esp32/api/v1/animals", json=animal_data)
    assert resp.status_code == 201
    animal_id = resp.json()["id"]
    
    # Read
    resp = requests.get(f"http://esp32/api/v1/animals/{animal_id}")
    assert resp.json()["species"] == "Python regius"
    
    # Update
    resp = requests.put(f"http://esp32/api/v1/animals/{animal_id}", 
                        json={"sex": "M"})
    assert resp.status_code == 200
    
    # Delete
    resp = requests.delete(f"http://esp32/api/v1/animals/{animal_id}")
    assert resp.status_code == 204
```

### 3. Load Tests

```bash
# Apache Bench
ab -n 1000 -c 10 http://esp32/api/v1/animals

# Locust
locust -f tests/load/locustfile.py --host=http://esp32
```

---

## 📚 Build & Deploy

### Development Build

```bash
# Setup environnement
source ~/esp/esp-idf/export.sh

# Configuration
idf.py menuconfig

# Build
idf.py build

# Flash + Monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

### Production Build

```bash
# Optimizations production
idf.py set-target esp32s3
idf.py menuconfig
    # → Compiler optimization: Optimize for performance (-O2)
    # → Compiler assertions: Disabled
    # → Log default level: Warning
    # → Secure Boot: Enabled
    # → Flash Encryption: Enabled

# Build
idf.py build

# Signer firmware
espsecure.py sign_data --keyfile key.pem \
    build/reptile-server.bin -o build/reptile-server-signed.bin

# Generate OTA package
cp build/reptile-server-signed.bin ota_package/firmware.bin
cd ota_package && zip ../firmware-v1.0.0.zip firmware.bin manifest.json
```

---

**Document Version**: 1.0  
**Date**: 2025-01-28  
**Hardware**: ESP32-S3-DevKitC-1-N32R16V  
**Framework**: ESP-IDF v6.1
