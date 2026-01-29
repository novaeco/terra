#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Minimal OneWire API shim.
 *
 * This placeholder allows the project to build when a full OneWire
 * driver is not present. Implement the functions with a real driver
 * or replace this module with an external component when available.
 */

typedef struct {
    uint8_t done;
} onewire_search_t;

void onewire_search_start(onewire_search_t *search);
bool onewire_search_next(onewire_search_t *search, uint8_t *address);

void onewire_reset(void);
void onewire_select(const uint8_t *address);
void onewire_write_byte(uint8_t byte);
void onewire_read_bytes(uint8_t *buffer, size_t length);
uint8_t onewire_crc8(const uint8_t *buffer, size_t length);

#ifdef __cplusplus
}
#endif
