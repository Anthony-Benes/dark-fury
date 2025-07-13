#pragma once

#include <defines.hpp>

#include <cstdarg>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if DF_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#else
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
#endif

class DF_API Logger {
public:
    static b8 Initialize();
    static void Shutdown();
    static void Fatal(const char* message, ...);
    static void Error(const char* message, ...);
    static void Warn(const char* message, ...);
    static void Info(const char* message, ...);
    static void Debug(const char* message, ...);
    static void Trace(const char* message, ...);
    b8 mRunning = false;
private:
  enum class Level {
    Fatal = 0,
    Error = 1,
    Warn = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};
    static Logger& Get();
    void log_out(Level level, const char* message, va_list args);
    Logger() = default;
    ~Logger() = default;
};