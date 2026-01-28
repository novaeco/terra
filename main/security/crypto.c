#include <stdio.h>
#include "crypto.h"

/*
 * Cryptography helper implementation.
 *
 * Wraps mbedTLS functions to compute SHA‑256 hashes and HMAC‑SHA256
 * message authentication codes.  Returns 0 on success and -1 on
 * failure.  The caller must provide a buffer of appropriate size
 * (32 bytes) for the output.
 */

#include "mbedtls/md.h"

int crypto_sha256(const unsigned char *data, size_t len, unsigned char *out_hash)
{
    if (!data || !out_hash) {
        return -1;
    }
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        return -1;
    }
    if (mbedtls_md(md_info, data, len, out_hash) != 0) {
        return -1;
    }
    return 0;
}

int crypto_hmac_sha256(const unsigned char *key, size_t key_len,
                       const unsigned char *data, size_t len,
                       unsigned char *out_mac)
{
    if (!key || !data || !out_mac) {
        return -1;
    }
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) {
        return -1;
    }
    if (mbedtls_md_hmac(md_info, key, key_len, data, len, out_mac) != 0) {
        return -1;
    }
    return 0;
}