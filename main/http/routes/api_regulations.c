#include <stdio.h>
#include "api_regulations.h"

int api_regulations_get_species(const char *name)
{
    printf("[api/regulations] GET species name=%s stub called\n", name);
    return 0;
}

int api_regulations_get_animal_status(const char *id)
{
    printf("[api/regulations] GET animal status id=%s stub called\n", id);
    return 0;
}

int api_regulations_get_alerts(void)
{
    printf("[api/regulations] GET alerts stub called\n");
    return 0;
}