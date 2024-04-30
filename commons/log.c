#include "log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

static tiadso_log_level_t current_log_level = 3;

void tiadso_log_set_level(tiadso_log_level_t level) {
    current_log_level = level;
}

void __tiadso_log(tiadso_log_level_t level, const char* tag, const char* fmt, ...) {
    FILE* __restrict__ stream = level == TIADSO_LOG_ERROR ? stderr : stdout;
    if (current_log_level >= level) {
        // Print tag
        fprintf(stream, "%s ", tag);

        // Print current datetime
        time_t now;
        time(&now);
        struct tm* pBrokenDownTime  = localtime(&now);
        fprintf(stream, "[%d-%02d-%02d %02d:%02d:%02d] > ", pBrokenDownTime->tm_year + 1900, pBrokenDownTime->tm_mon + 1, pBrokenDownTime->tm_mday, pBrokenDownTime->tm_hour, pBrokenDownTime->tm_min, pBrokenDownTime->tm_sec);

        // Pass fmt and variadic arguments to printf
        va_list args;

        if (fmt != NULL) {
            va_start(args, fmt);
            vfprintf(stream, fmt, args);
            va_end(args);
        } else {
            va_start(args, fmt);
            vfprintf(stream, "%s", args);
            va_end(args);
        }
        fprintf(stream, "\n");
    }
}

void tiadso_loge(const char* __restrict tag, const char* __restrict fmt, ...) {
    __tiadso_log(TIADSO_LOG_ERROR, tag, fmt);
}
void tiadso_logw(const char* __restrict tag, const char* __restrict fmt, ...) {
    __tiadso_log(TIADSO_LOG_WARN, tag, fmt);
}
void tiadso_logi(const char* __restrict tag, const char* __restrict fmt, ...) {
    __tiadso_log(TIADSO_LOG_INFO, tag, fmt);
}
void tiadso_logd(const char* __restrict tag, const char* __restrict fmt, ...) {
    __tiadso_log(TIADSO_LOG_DEBUG, tag, fmt);
}
void tiadso_logv(const char* __restrict tag, const char* __restrict fmt, ...) {
    __tiadso_log(TIADSO_LOG_VERBOSE, tag, fmt);
}