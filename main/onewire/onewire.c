#include "onewire.h"

/*
 * Placeholder implementation. All functions are no-ops so the build
 * succeeds without a real OneWire driver. Replace with a hardware
 * backed implementation for production use.
 */

void onewire_search_start(onewire_search_t *search)
{
    if (search) {
        search->done = 1;
    }
}

bool onewire_search_next(onewire_search_t *search, uint8_t *address)
{
    (void)search;
    (void)address;
    return false;
}

void onewire_reset(void)
{
}

void onewire_select(const uint8_t *address)
{
    (void)address;
}

void onewire_write_byte(uint8_t byte)
{
    (void)byte;
}

void onewire_read_bytes(uint8_t *buffer, size_t length)
{
    if (!buffer) {
        return;
    }
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = 0;
    }
}

uint8_t onewire_crc8(const uint8_t *buffer, size_t length)
{
    (void)buffer;
    (void)length;
    return 0;
}
