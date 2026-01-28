#include <time.h>
#include "datetime.h"

/*
 * Return the current Unix timestamp (seconds since epoch).
 */
uint32_t datetime_now(void)
{
    return (uint32_t)time(NULL);
}