#include <core/logger.hpp>
#include <core/asserts.hpp>
#include <platform/platform.hpp>

#include <stdio.h>
#include <string.h>

Logger& Logger::Get() {
    static Logger sLog;
    return sLog;
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    Logger::Fatal("Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}

b8 Logger::Initialize() {
    Logger log = Logger::Get();
    if (log.mRunning) {
        // Already initialized. Shouldn't call again.
        return false;
    }
    log.mRunning = true;
    // TODO: create log file
    return true;
}

void Logger::Shutdown() {
    Logger log = Logger::Get();
    if (!log.mRunning) {
        // Not initialized. Can't shut down properly.
        return;
    }
    // TODO: cleanup logging/write queued entries
    log.mRunning = false;
}

void Logger::Fatal(const char* message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    Logger::Get().log_out(Level::Fatal, message, arg_ptr);
    va_end(arg_ptr);
}

void Logger::Error(const char* message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    Logger::Get().log_out(Level::Error, message, arg_ptr);
    va_end(arg_ptr);
}

void Logger::Warn(const char* message, ...) {
    if constexpr (LOG_WARN_ENABLED) {
        va_list arg_ptr;
        va_start(arg_ptr, message);
        Logger::Get().log_out(Level::Warn, message, arg_ptr);
        va_end(arg_ptr);
    }
}

void Logger::Info(const char* message, ...) {
    if constexpr (LOG_INFO_ENABLED) {
        va_list arg_ptr;
        va_start(arg_ptr, message);
        Logger::Get().log_out(Level::Info, message, arg_ptr);
        va_end(arg_ptr);
    }
}

void Logger::Debug(const char* message, ...) {
    if constexpr (LOG_DEBUG_ENABLED) {
        va_list arg_ptr;
        va_start(arg_ptr, message);
        Logger::Get().log_out(Level::Debug, message, arg_ptr);
        va_end(arg_ptr);
    }
}

void Logger::Trace(const char* message, ...) {
    if constexpr (LOG_TRACE_ENABLED) {
        va_list arg_ptr;
        va_start(arg_ptr, message);
        Logger::Get().log_out(Level::Trace, message, arg_ptr);
        va_end(arg_ptr);
    }
}

void Logger::log_out(Level level, const char* message, va_list args) {
    const char* level_strings[6] = {
                                     "[FATAL]: ",
                                     "[ERROR]: ",
                                     "[WARN]:  ",
                                     "[INFO]:  ",
                                     "[DEBUG]: ",
                                     "[TRACE]: " };
    const auto levelInt = static_cast<u8>(level);

    const i32 MSG_LENGTH = 32000;
    char buff_message[MSG_LENGTH];
    memset(buff_message, 0, sizeof(buff_message));
    vsnprintf(buff_message, MSG_LENGTH, message, args);

    char out_message[MSG_LENGTH + 1];
    sprintf(out_message, "%s%s\n", level_strings[levelInt], buff_message);

    if (level <= Level::Error) {
        platform_console_write_error(out_message, levelInt);
    }
    else {
        platform_console_write(out_message, levelInt);
    }
}