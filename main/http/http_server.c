#include <stdio.h>
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
