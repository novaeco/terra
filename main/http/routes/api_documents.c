#include <stdio.h>
#include "api_documents.h"

int api_documents_generate(void)
{
    printf("[api/documents] POST generate stub called\n");
    return 0;
}

int api_documents_download(const char *id)
{
    printf("[api/documents] GET download id=%s stub called\n", id);
    return 0;
}