#ifndef __COMMONS_LOG_H
#define __COMMONS_LOG_H

typedef enum {
    TIADSO_LOG_NONE,       /*!< No log output */
    TIADSO_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    TIADSO_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    TIADSO_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    TIADSO_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    TIADSO_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} tiadso_log_level_t;

void tiadso_log_set_level(tiadso_log_level_t level);
void tiadso_log(tiadso_log_level_t level, const char* tag, const char* fmt, ...);

void tiadso_loge(const char* __restrict tag, const char* __restrict fmt, ...);
void tiadso_logw(const char* __restrict tag, const char* __restrict fmt, ...);
void tiadso_logi(const char* __restrict tag, const char* __restrict fmt, ...);
void tiadso_logd(const char* __restrict tag, const char* __restrict fmt, ...);
void tiadso_logv(const char* __restrict tag, const char* __restrict fmt, ...);

#define TIADSO_LOGE(tag, ...) tiadso_loge(tag, __VA_ARGS__)
#define TIADSO_LOGW(tag, ...) tiadso_logw(tag, __VA_ARGS__)
#define TIADSO_LOGI(tag, ...) tiadso_logi(tag, __VA_ARGS__)
#define TIADSO_LOGD(tag, ...) tiadso_logd(tag, __VA_ARGS__)
#define TIADSO_LOGV(tag, ...) tiadso_logv(tag, __VA_ARGS__)

#endif //__COMMONS_LOG_H