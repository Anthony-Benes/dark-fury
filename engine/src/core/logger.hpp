#pragma once

#include <defines.hpp>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if DF_RELASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_DEBUG_ENABLED 0
#else
#define LOG_DEBUG_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_INFO  = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

b8 initialize_logging();
void shutdown_logging();

DF_API void log_output(log_level level, const char* message, ...);

// Outgoing Log Access
#define DF_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef DF_ERROR
#define DF_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define DF_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define DF_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define DF_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define DF_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define DF_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define DF_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define DF_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define DF_TRACE(message, ...)
#endif
