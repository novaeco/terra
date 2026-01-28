#ifndef LOGGER_H
#define LOGGER_H

/*
 * Simple logger interface.  In the full project this would wrap
 * ESP‑IDF logging macros and support different log levels.
 */
void log_info(const char *tag, const char *fmt, ...);
void log_warn(const char *tag, const char *fmt, ...);
void log_error(const char *tag, const char *fmt, ...);

#endif /* LOGGER_H */