#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define LOG_TRACE 0
#define LOG_DEBUG 1
#define LOG_INFO 2
#define LOG_WARN 3
#define LOG_ERROR 4
#define LOG_FATAL 5

void set_log_level(int level);
void log_message(int level, const char *format, ...);

#endif
