#include <stdio.h>
#include "api_animals.h"

int api_animals_get_all(void)
{
    printf("[api/animals] GET all stub called\n");
    return 0;
}

int api_animals_create(void)
{
    printf("[api/animals] POST create stub called\n");
    return 0;
}

int api_animals_get(const char *id)
{
    printf("[api/animals] GET id=%s stub called\n", id);
    return 0;
}

int api_animals_update(const char *id)
{
    printf("[api/animals] PUT id=%s stub called\n", id);
    return 0;
}

int api_animals_delete(const char *id)
{
    printf("[api/animals] DELETE id=%s stub called\n", id);
    return 0;
}