#include "auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "mbedtls/md.h"
#include "mbedtls/base64.h"

/*
 * Implementation of JWT generation and verification.  This code is
 * adapted from the architecture specification.  It builds a JWT with
 * a JSON header and payload, signs it using HMAC-SHA256 and encodes
 * the result using Base64 URL encoding.  Note that a more robust
 * implementation would include error handling and dynamic memory
 * management; this example focuses on demonstrating the core logic.
 */

// Secret key used for HMAC-SHA256 signature.  In a real project
// store this securely in NVS or another secure location.
static const char *JWT_SECRET = "supersecret";

// Token expiry in seconds (24 hours)
#define JWT_EXPIRY_SEC (24 * 60 * 60)

// Forward declaration of helper
static char *base64url_encode(const unsigned char *data, size_t len);

static char *base64url_encode(const unsigned char *data, size_t len)
{
    size_t out_len = 0;
    // First call to determine required length
    mbedtls_base64_encode(NULL, 0, &out_len, data, len);
    // Allocate buffer (add a few bytes for safety)
    unsigned char *buffer = calloc(1, out_len + 4);
    if (!buffer) {
        return NULL;
    }
    mbedtls_base64_encode(buffer, out_len + 4, &out_len, data, len);
    // Convert to URL safe and remove padding
    for (size_t i = 0; i < out_len; i++) {
        if (buffer[i] == '+') buffer[i] = '-';
        else if (buffer[i] == '/') buffer[i] = '_';
    }
    while (out_len > 0 && buffer[out_len - 1] == '=') {
        buffer[--out_len] = '\0';
    }
    return (char *)buffer;
}

int auth_jwt_generate(const char *username, const char *role, char *out_token, size_t max_len)
{
    if (!out_token || max_len == 0) {
        return -1;
    }

    // 1. Create header JSON
    cJSON *header = cJSON_CreateObject();
    cJSON_AddStringToObject(header, "alg", "HS256");
    cJSON_AddStringToObject(header, "typ", "JWT");

    // 2. Create payload JSON
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "sub", username);
    cJSON_AddStringToObject(payload, "role", role);
    cJSON_AddNumberToObject(payload, "iat", (double)time(NULL));
    cJSON_AddNumberToObject(payload, "exp", (double)(time(NULL) + JWT_EXPIRY_SEC));

    // 3. Encode header and payload
    char *header_str = cJSON_PrintUnformatted(header);
    char *payload_str = cJSON_PrintUnformatted(payload);
    char *header_b64 = base64url_encode((const unsigned char *)header_str, strlen(header_str));
    char *payload_b64 = base64url_encode((const unsigned char *)payload_str, strlen(payload_str));
    cJSON_Delete(header);
    cJSON_Delete(payload);
    free(header_str);
    free(payload_str);
    if (!header_b64 || !payload_b64) {
        free(header_b64);
        free(payload_b64);
        return -1;
    }

    // 4. Prepare signature input
    size_t input_len = strlen(header_b64) + 1 + strlen(payload_b64);
    char *signature_input = malloc(input_len + 1);
    if (!signature_input) {
        free(header_b64);
        free(payload_b64);
        return -1;
    }
    sprintf(signature_input, "%s.%s", header_b64, payload_b64);

    // 5. Compute HMAC-SHA256 signature
    unsigned char signature[32];
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_hmac(md_info, (const unsigned char *)JWT_SECRET, strlen(JWT_SECRET),
                    (const unsigned char *)signature_input, strlen(signature_input),
                    signature);

    char *signature_b64 = base64url_encode(signature, sizeof(signature));
    free(signature_input);
    if (!signature_b64) {
        free(header_b64);
        free(payload_b64);
        return -1;
    }

    // 6. Assemble final token
    size_t token_len = strlen(header_b64) + strlen(payload_b64) + strlen(signature_b64) + 3;
    char *token = malloc(token_len);
    if (!token) {
        free(header_b64);
        free(payload_b64);
        free(signature_b64);
        return -1;
    }
    sprintf(token, "%s.%s.%s", header_b64, payload_b64, signature_b64);

    // Copy to output buffer
    strncpy(out_token, token, max_len - 1);
    out_token[max_len - 1] = '\0';

    free(header_b64);
    free(payload_b64);
    free(signature_b64);
    free(token);

    return 0;
}

int auth_jwt_verify(const char *token)
{
    // A full JWT verification would parse the token, verify the signature
    // with the secret, and check expiry times.  Here we simply check
    // that the token contains two dots (header.payload.signature).
    if (!token) {
        return -1;
    }
    int dots = 0;
    for (const char *p = token; *p; ++p) {
        if (*p == '.') {
            dots++;
        }
    }
    return (dots == 2) ? 0 : -1;
}