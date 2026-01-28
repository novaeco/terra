#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

static void vlog(const char *level, const char *tag, const char *fmt, va_list args)
{
    printf("[%s][%s] ", level, tag);
    vprintf(fmt, args);
    printf("\n");
}

void log_info(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlog("INFO", tag, fmt, args);
    va_end(args);
}

void log_warn(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlog("WARN", tag, fmt, args);
    va_end(args);
}

void log_error(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlog("ERROR", tag, fmt, args);
    va_end(args);
}