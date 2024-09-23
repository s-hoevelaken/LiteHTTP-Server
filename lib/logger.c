#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logger.h"

#define LOG_FILE "logs/server.log"

int log_level = LOG_INFO;

void set_log_level(int level) {
    log_level = level;
}

char *current_time_str() {
    static char time_buffer[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", t);
    return time_buffer;
}

void log_message(int level, const char *format, ...) {
    if (level < log_level) {
        return;
    }

    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        // create log file if doesnt exist
        log_file = fopen(LOG_FILE, "w");
        if (!log_file) {
            perror("Failed to create log file");
            return;
        }
    }

    const char *level_str;
    switch (level) {
        case LOG_TRACE: level_str = "TRACE"; break;
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO:  level_str = "INFO"; break;
        case LOG_WARN:  level_str = "WARN"; break;
        case LOG_ERROR: level_str = "ERROR"; break;
        case LOG_FATAL: level_str = "FATAL"; break;
        default:        level_str = "INFO"; break;
    }

    va_list args;
    va_start(args, format);
    fprintf(log_file, "[%s] [%s] ", current_time_str(), level_str);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    va_end(args);

    fclose(log_file);
}