#ifndef CRYPTO_H
#define CRYPTO_H

/*
 * Cryptography helpers.  In a production build this module would
 * wrap mbedTLS or another crypto library to perform hashing,
 * HMAC, encryption and decryption.  Here we simply provide stub
 * functions to illustrate the interface.
 */

int crypto_sha256(const unsigned char *data, size_t len, unsigned char *out_hash);
int crypto_hmac_sha256(const unsigned char *key, size_t key_len,
                       const unsigned char *data, size_t len,
                       unsigned char *out_mac);

#endif /* CRYPTO_H */