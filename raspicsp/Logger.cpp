#include <stdarg.h>
#include <stdio.h>
#include "Logger.h"

int Logger::tracing = 0;

void Logger::log(const char *category, const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    fprintf(stdout, "[%-8s] ", category);
    vfprintf(stdout, format, argptr);
    fprintf(stdout, "\n");
    va_end(argptr);
}

void Logger::trace(const char *category, const char *format, ...) {
    if (tracing) {
        va_list argptr;
        va_start(argptr, format);
        fprintf(stdout, "<%-8s> ", category);
        vfprintf(stdout, format, argptr);
        fprintf(stdout, "\n");
        va_end(argptr);
    }
}

void Logger::enable_tracing() {
    tracing = 1;
}

int Logger::is_tracing() {
    return tracing;
}
