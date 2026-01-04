#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

#define LV_CONF_INCLUDE_SIMPLE 1

/* Couleurs */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

/* STDLIB */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/* OS / Tick */
#define LV_USE_OS   LV_OS_FREERTOS

/* Logs */
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO
#define LV_LOG_PRINTF 1

/* Fonts */
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_20 1

#endif // LV_CONF_H
