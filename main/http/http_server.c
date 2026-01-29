#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_server.h"

/*
 * Simple HTTP server implementation.
 *
 * This server uses the ESP‑IDF HTTP server component to register a
 * handful of REST API endpoints.  Only a single endpoint
 * (/api/v1/system/stats) is implemented to demonstrate the basic
 * pattern.  Additional endpoints can be added by registering
 * further httpd_uri_t structures.  For TLS support use
 * httpd_ssl_start() instead of httpd_start().
 */

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "utils/logger.h"
#include "storage/nvs_manager.h"

static const char *TAG_HTTP = "http";
#define WIFI_CRED_MAX_BODY 256
#define CONFIG_MAX_BODY 768
#define CONFIG_VALUE_MAX 64
#define CONFIG_PORT_MAX 6

static const char *CONFIG_PAGE_HTML =
    "<!doctype html>\n"
    "<html lang=\"fr\">\n"
    "<head>\n"
    "  <meta charset=\"utf-8\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n"
    "  <title>ESP32 Reptile Manager - Configuration</title>\n"
    "  <style>\n"
    "    body{font-family:system-ui,-apple-system,Segoe UI,Roboto,Ubuntu,\"Helvetica Neue\",Arial,sans-serif;margin:0;background:#0f172a;color:#e2e8f0}\n"
    "    main{max-width:720px;margin:0 auto;padding:24px}\n"
    "    h1{font-size:1.5rem;margin-bottom:0.5rem}\n"
    "    h2{font-size:1.1rem;margin:24px 0 8px}\n"
    "    .card{background:#111827;border:1px solid #1f2937;border-radius:12px;padding:16px;margin-bottom:16px}\n"
    "    label{display:block;font-size:0.9rem;margin-top:10px}\n"
    "    input{width:100%;padding:10px;border-radius:8px;border:1px solid #334155;background:#0b1220;color:#e2e8f0}\n"
    "    button{margin-top:16px;padding:10px 16px;border-radius:8px;border:none;background:#38bdf8;color:#0f172a;font-weight:600;cursor:pointer}\n"
    "    .hint{font-size:0.8rem;color:#94a3b8}\n"
    "    .status{margin-top:12px;font-size:0.9rem}\n"
    "  </style>\n"
    "</head>\n"
    "<body>\n"
    "  <main>\n"
    "    <h1>Configuration ESP32 Reptile Manager</h1>\n"
    "    <p class=\"hint\">Remplissez uniquement les champs à modifier. Laissez vide pour conserver la valeur actuelle.</p>\n"
    "    <form id=\"configForm\" class=\"card\">\n"
    "      <h2>Wi-Fi</h2>\n"
    "      <label>SSID\n"
    "        <input id=\"wifi_ssid\" name=\"wifi_ssid\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Mot de passe\n"
    "        <input id=\"wifi_password\" name=\"wifi_password\" type=\"password\" />\n"
    "      </label>\n"
    "      <label class=\"hint\"><input id=\"wifi_password_clear\" type=\"checkbox\" /> Envoyer un mot de passe vide (réseau ouvert)</label>\n"
    "\n"
    "      <h2>Serveur</h2>\n"
    "      <label>Hôte\n"
    "        <input id=\"server_host\" name=\"server_host\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Port\n"
    "        <input id=\"server_port\" name=\"server_port\" type=\"number\" min=\"1\" max=\"65535\" />\n"
    "      </label>\n"
    "      <label>Identifiant\n"
    "        <input id=\"server_user\" name=\"server_user\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Mot de passe\n"
    "        <input id=\"server_password\" name=\"server_password\" type=\"password\" />\n"
    "      </label>\n"
    "      <label class=\"hint\"><input id=\"server_password_clear\" type=\"checkbox\" /> Envoyer un mot de passe vide</label>\n"
    "\n"
    "      <h2>Base de données</h2>\n"
    "      <label>Hôte\n"
    "        <input id=\"db_host\" name=\"db_host\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Port\n"
    "        <input id=\"db_port\" name=\"db_port\" type=\"number\" min=\"1\" max=\"65535\" />\n"
    "      </label>\n"
    "      <label>Nom de base\n"
    "        <input id=\"db_name\" name=\"db_name\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Utilisateur\n"
    "        <input id=\"db_user\" name=\"db_user\" type=\"text\" />\n"
    "      </label>\n"
    "      <label>Mot de passe\n"
    "        <input id=\"db_password\" name=\"db_password\" type=\"password\" />\n"
    "      </label>\n"
    "      <label class=\"hint\"><input id=\"db_password_clear\" type=\"checkbox\" /> Envoyer un mot de passe vide</label>\n"
    "\n"
    "      <button type=\"submit\">Enregistrer</button>\n"
    "      <div id=\"status\" class=\"status\"></div>\n"
    "    </form>\n"
    "  </main>\n"
    "  <script>\n"
    "    const statusEl = document.getElementById('status');\n"
    "    const getValue = (id) => document.getElementById(id).value.trim();\n"
    "    const getChecked = (id) => document.getElementById(id).checked;\n"
    "\n"
    "    async function loadConfig() {\n"
    "      try {\n"
    "        const res = await fetch('/api/v1/config');\n"
    "        if (!res.ok) throw new Error('Erreur chargement');\n"
    "        const data = await res.json();\n"
    "        document.getElementById('wifi_ssid').value = data.wifi?.ssid || '';\n"
    "        document.getElementById('server_host').value = data.server?.host || '';\n"
    "        document.getElementById('server_port').value = data.server?.port || '';\n"
    "        document.getElementById('server_user').value = data.server?.user || '';\n"
    "        document.getElementById('db_host').value = data.database?.host || '';\n"
    "        document.getElementById('db_port').value = data.database?.port || '';\n"
    "        document.getElementById('db_name').value = data.database?.name || '';\n"
    "        document.getElementById('db_user').value = data.database?.user || '';\n"
    "      } catch (err) {\n"
    "        statusEl.textContent = 'Impossible de charger la configuration.';\n"
    "      }\n"
    "    }\n"
    "\n"
    "    document.getElementById('configForm').addEventListener('submit', async (event) => {\n"
    "      event.preventDefault();\n"
    "      statusEl.textContent = 'Enregistrement...';\n"
    "      const payload = { wifi: {}, server: {}, database: {} };\n"
    "\n"
    "      const wifiSsid = getValue('wifi_ssid');\n"
    "      const wifiPassword = document.getElementById('wifi_password').value;\n"
    "      if (wifiSsid) payload.wifi.ssid = wifiSsid;\n"
    "      if (wifiPassword) payload.wifi.password = wifiPassword;\n"
    "      if (!wifiPassword && getChecked('wifi_password_clear')) payload.wifi.password = '';\n"
    "\n"
    "      const serverHost = getValue('server_host');\n"
    "      const serverPort = getValue('server_port');\n"
    "      const serverUser = getValue('server_user');\n"
    "      const serverPassword = document.getElementById('server_password').value;\n"
    "      if (serverHost) payload.server.host = serverHost;\n"
    "      if (serverPort) payload.server.port = Number(serverPort);\n"
    "      if (serverUser) payload.server.user = serverUser;\n"
    "      if (serverPassword) payload.server.password = serverPassword;\n"
    "      if (!serverPassword && getChecked('server_password_clear')) payload.server.password = '';\n"
    "\n"
    "      const dbHost = getValue('db_host');\n"
    "      const dbPort = getValue('db_port');\n"
    "      const dbName = getValue('db_name');\n"
    "      const dbUser = getValue('db_user');\n"
    "      const dbPassword = document.getElementById('db_password').value;\n"
    "      if (dbHost) payload.database.host = dbHost;\n"
    "      if (dbPort) payload.database.port = Number(dbPort);\n"
    "      if (dbName) payload.database.name = dbName;\n"
    "      if (dbUser) payload.database.user = dbUser;\n"
    "      if (dbPassword) payload.database.password = dbPassword;\n"
    "      if (!dbPassword && getChecked('db_password_clear')) payload.database.password = '';\n"
    "\n"
    "      try {\n"
    "        const res = await fetch('/api/v1/config', {\n"
    "          method: 'POST',\n"
    "          headers: { 'Content-Type': 'application/json' },\n"
    "          body: JSON.stringify(payload),\n"
    "        });\n"
    "        const data = await res.json();\n"
    "        if (!res.ok) throw new Error(data?.error || 'Erreur');\n"
    "        statusEl.textContent = data.action === 'reboot'\n"
    "          ? 'Configuration enregistrée. Redémarrage en cours...'\n"
    "          : 'Configuration enregistrée.';\n"
    "      } catch (err) {\n"
    "        statusEl.textContent = 'Échec enregistrement: ' + err.message;\n"
    "      }\n"
    "    });\n"
    "\n"
    "    loadConfig();\n"
    "  </script>\n"
    "</body>\n"
    "</html>\n";

typedef struct {
    char wifi_ssid[sizeof(((wifi_config_t *)0)->sta.ssid)];
    char server_host[CONFIG_VALUE_MAX];
    char server_port[CONFIG_PORT_MAX];
    char server_user[CONFIG_VALUE_MAX];
    char db_host[CONFIG_VALUE_MAX];
    char db_port[CONFIG_PORT_MAX];
    char db_name[CONFIG_VALUE_MAX];
    char db_user[CONFIG_VALUE_MAX];
} config_snapshot_t;

static void nvs_get_str_or_empty(const char *key, char *value, size_t max_len)
{
    if (nvsman_get_str(key, value, max_len) != 0) {
        value[0] = '\0';
    }
}

static bool set_nvs_str_if_valid(const char *key, const cJSON *item, size_t max_len,
                                 bool allow_empty, bool *changed)
{
    if (!cJSON_IsString(item)) {
        return true;
    }
    const char *value = item->valuestring ? item->valuestring : "";
    size_t len = strlen(value);
    if (len == 0 && !allow_empty) {
        return true;
    }
    if (len > max_len) {
        return false;
    }
    if (nvsman_set_str(key, value) != 0) {
        return false;
    }
    if (changed) {
        *changed = true;
    }
    return true;
}

static bool set_nvs_port_if_valid(const char *key, const cJSON *item, bool *changed)
{
    if (!item) {
        return true;
    }
    long port = -1;
    if (cJSON_IsNumber(item)) {
        port = (long)item->valuedouble;
    } else if (cJSON_IsString(item)) {
        const char *value = item->valuestring;
        if (!value || value[0] == '\0') {
            return true;
        }
        char *endptr = NULL;
        port = strtol(value, &endptr, 10);
        if (!endptr || *endptr != '\0') {
            return false;
        }
    } else {
        return true;
    }
    if (port < 1 || port > 65535) {
        return false;
    }
    char port_str[CONFIG_PORT_MAX];
    snprintf(port_str, sizeof(port_str), "%ld", port);
    if (nvsman_set_str(key, port_str) != 0) {
        return false;
    }
    if (changed) {
        *changed = true;
    }
    return true;
}

// Handler for GET /api/v1/system/stats
static esp_err_t stats_get_handler(httpd_req_t *req)
{
    // Build a JSON response with basic system stats
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "uptime", (double)esp_timer_get_time() / 1e6);
    cJSON_AddNumberToObject(root, "heap_free", (double)esp_get_free_heap_size());
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!json_str) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "JSON encode failed");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, HTTPD_RESP_USE_STRLEN);
    free(json_str);
    return ESP_OK;
}

static esp_err_t config_page_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, CONFIG_PAGE_HTML, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t config_get_handler(httpd_req_t *req)
{
    config_snapshot_t config = { 0 };
    nvs_get_str_or_empty("wifi_ssid", config.wifi_ssid, sizeof(config.wifi_ssid));
    nvs_get_str_or_empty("srv_host", config.server_host, sizeof(config.server_host));
    nvs_get_str_or_empty("srv_port", config.server_port, sizeof(config.server_port));
    nvs_get_str_or_empty("srv_user", config.server_user, sizeof(config.server_user));
    nvs_get_str_or_empty("db_host", config.db_host, sizeof(config.db_host));
    nvs_get_str_or_empty("db_port", config.db_port, sizeof(config.db_port));
    nvs_get_str_or_empty("db_name", config.db_name, sizeof(config.db_name));
    nvs_get_str_or_empty("db_user", config.db_user, sizeof(config.db_user));

    cJSON *root = cJSON_CreateObject();
    cJSON *wifi = cJSON_AddObjectToObject(root, "wifi");
    cJSON_AddStringToObject(wifi, "ssid", config.wifi_ssid);

    cJSON *server = cJSON_AddObjectToObject(root, "server");
    cJSON_AddStringToObject(server, "host", config.server_host);
    cJSON_AddStringToObject(server, "port", config.server_port);
    cJSON_AddStringToObject(server, "user", config.server_user);
    cJSON_AddStringToObject(server, "password", "");

    cJSON *database = cJSON_AddObjectToObject(root, "database");
    cJSON_AddStringToObject(database, "host", config.db_host);
    cJSON_AddStringToObject(database, "port", config.db_port);
    cJSON_AddStringToObject(database, "name", config.db_name);
    cJSON_AddStringToObject(database, "user", config.db_user);
    cJSON_AddStringToObject(database, "password", "");

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!json_str) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "JSON encode failed");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, HTTPD_RESP_USE_STRLEN);
    free(json_str);
    return ESP_OK;
}

static esp_err_t config_post_handler(httpd_req_t *req)
{
    if (req->content_len <= 0 || req->content_len > CONFIG_MAX_BODY) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid content length");
        return ESP_FAIL;
    }

    char body[CONFIG_MAX_BODY + 1];
    int received = httpd_req_recv(req, body, req->content_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read body");
        return ESP_FAIL;
    }
    body[received] = '\0';

    cJSON *root = cJSON_ParseWithLength(body, received);
    if (!root) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    bool wifi_changed = false;
    bool config_changed = false;
    const cJSON *wifi = cJSON_GetObjectItemCaseSensitive(root, "wifi");
    if (cJSON_IsObject(wifi)) {
        const cJSON *ssid = cJSON_GetObjectItemCaseSensitive(wifi, "ssid");
        const cJSON *password = cJSON_GetObjectItemCaseSensitive(wifi, "password");
        size_t max_ssid_len = sizeof(((wifi_config_t *)0)->sta.ssid) - 1;
        size_t max_pass_len = sizeof(((wifi_config_t *)0)->sta.password) - 1;

        if (!set_nvs_str_if_valid("wifi_ssid", ssid, max_ssid_len, false, &wifi_changed) ||
            !set_nvs_str_if_valid("wifi_pass", password, max_pass_len, true, &wifi_changed)) {
            cJSON_Delete(root);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid Wi-Fi fields");
            return ESP_FAIL;
        }
    }

    const cJSON *server = cJSON_GetObjectItemCaseSensitive(root, "server");
    if (cJSON_IsObject(server)) {
        if (!set_nvs_str_if_valid("srv_host",
                                  cJSON_GetObjectItemCaseSensitive(server, "host"),
                                  CONFIG_VALUE_MAX - 1, false, &config_changed) ||
            !set_nvs_port_if_valid("srv_port",
                                   cJSON_GetObjectItemCaseSensitive(server, "port"),
                                   &config_changed) ||
            !set_nvs_str_if_valid("srv_user",
                                  cJSON_GetObjectItemCaseSensitive(server, "user"),
                                  CONFIG_VALUE_MAX - 1, false, &config_changed) ||
            !set_nvs_str_if_valid("srv_pass",
                                  cJSON_GetObjectItemCaseSensitive(server, "password"),
                                  CONFIG_VALUE_MAX - 1, true, &config_changed)) {
            cJSON_Delete(root);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid server fields");
            return ESP_FAIL;
        }
    }

    const cJSON *database = cJSON_GetObjectItemCaseSensitive(root, "database");
    if (cJSON_IsObject(database)) {
        if (!set_nvs_str_if_valid("db_host",
                                  cJSON_GetObjectItemCaseSensitive(database, "host"),
                                  CONFIG_VALUE_MAX - 1, false, &config_changed) ||
            !set_nvs_port_if_valid("db_port",
                                   cJSON_GetObjectItemCaseSensitive(database, "port"),
                                   &config_changed) ||
            !set_nvs_str_if_valid("db_name",
                                  cJSON_GetObjectItemCaseSensitive(database, "name"),
                                  CONFIG_VALUE_MAX - 1, false, &config_changed) ||
            !set_nvs_str_if_valid("db_user",
                                  cJSON_GetObjectItemCaseSensitive(database, "user"),
                                  CONFIG_VALUE_MAX - 1, false, &config_changed) ||
            !set_nvs_str_if_valid("db_pass",
                                  cJSON_GetObjectItemCaseSensitive(database, "password"),
                                  CONFIG_VALUE_MAX - 1, true, &config_changed)) {
            cJSON_Delete(root);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid database fields");
            return ESP_FAIL;
        }
    }

    cJSON_Delete(root);
    httpd_resp_set_type(req, "application/json");
    if (wifi_changed) {
        httpd_resp_sendstr(req, "{\"status\":\"ok\",\"action\":\"reboot\"}");
        vTaskDelay(pdMS_TO_TICKS(200));
        esp_restart();
        return ESP_OK;
    }
    httpd_resp_sendstr(req, "{\"status\":\"ok\",\"action\":\"none\"}");
    if (config_changed) {
        ESP_LOGI(TAG_HTTP, "Configuration updated (server/db).");
    }
    return ESP_OK;
}

static esp_err_t wifi_credentials_post_handler(httpd_req_t *req)
{
    if (req->content_len <= 0 || req->content_len > WIFI_CRED_MAX_BODY) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid content length");
        return ESP_FAIL;
    }

    char body[WIFI_CRED_MAX_BODY + 1];
    int received = httpd_req_recv(req, body, req->content_len);
    if (received <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read body");
        return ESP_FAIL;
    }
    body[received] = '\0';

    cJSON *root = cJSON_ParseWithLength(body, received);
    if (!root) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    const cJSON *ssid = cJSON_GetObjectItemCaseSensitive(root, "ssid");
    const cJSON *password = cJSON_GetObjectItemCaseSensitive(root, "password");
    if (!cJSON_IsString(ssid) || ssid->valuestring[0] == '\0' ||
        !cJSON_IsString(password)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing ssid/password");
        return ESP_FAIL;
    }

    size_t max_ssid_len = sizeof(((wifi_config_t *)0)->sta.ssid) - 1;
    size_t max_pass_len = sizeof(((wifi_config_t *)0)->sta.password) - 1;
    if (strlen(ssid->valuestring) > max_ssid_len ||
        strlen(password->valuestring) > max_pass_len) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SSID/password too long");
        return ESP_FAIL;
    }

    if (nvsman_set_str("wifi_ssid", ssid->valuestring) != 0 ||
        nvsman_set_str("wifi_pass", password->valuestring) != 0) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to store credentials");
        return ESP_FAIL;
    }

    cJSON_Delete(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\",\"action\":\"reboot\"}");

    vTaskDelay(pdMS_TO_TICKS(200));
    esp_restart();
    return ESP_OK;
}

int http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_HTTP, "Failed to start HTTP server: %s", esp_err_to_name(err));
        return -1;
    }
    // Register the /api/v1/system/stats endpoint
    httpd_uri_t stats_uri = {
        .uri = "/api/v1/system/stats",
        .method = HTTP_GET,
        .handler = stats_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &stats_uri);

    httpd_uri_t config_page_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = config_page_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &config_page_uri);

    httpd_uri_t config_get_uri = {
        .uri = "/api/v1/config",
        .method = HTTP_GET,
        .handler = config_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &config_get_uri);

    httpd_uri_t config_post_uri = {
        .uri = "/api/v1/config",
        .method = HTTP_POST,
        .handler = config_post_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &config_post_uri);

    httpd_uri_t wifi_creds_uri = {
        .uri = "/api/v1/wifi/credentials",
        .method = HTTP_POST,
        .handler = wifi_credentials_post_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &wifi_creds_uri);
    ESP_LOGI(TAG_HTTP, "HTTP server started on port %d", config.server_port);
    return 0;
}
