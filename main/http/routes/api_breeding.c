#include <stdio.h>
#include "api_breeding.h"

int api_breeding_get_cycles(void)
{
    printf("[api/breeding] GET cycles stub called\n");
    return 0;
}

int api_breeding_create_cycle(void)
{
    printf("[api/breeding] POST create cycle stub called\n");
    return 0;
}

int api_breeding_get_cycle(const char *id)
{
    printf("[api/breeding] GET cycle id=%s stub called\n", id);
    return 0;
}

int api_breeding_record_mating(const char *id)
{
    printf("[api/breeding] POST mating id=%s stub called\n", id);
    return 0;
}

int api_breeding_record_clutch(const char *id)
{
    printf("[api/breeding] POST clutch id=%s stub called\n", id);
    return 0;
}

int api_breeding_record_hatching(const char *id)
{
    printf("[api/breeding] POST hatching id=%s stub called\n", id);
    return 0;
}