#include "logger.hpp"
#include "asserts.hpp"
#include "platform/platform.hpp"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}

b8 initialize_logging() {
    // TODO: create log file
    return TRUE;
}

void shutdown_logging() {
    // TODO: cleanup logging/write queued entries
}

void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {
                                     "[FATAL]: ",
                                     "[ERROR]: ",
                                     "[WARN]: ",
                                     "[INFO]: ",
                                     "[DEBUG]: ",
                                     "[TRACE]: "};
    b8 is_error = level < LOG_LEVEL_WARN;

    const i32 MSG_LENGTH = 32000;
    char buff_message[MSG_LENGTH];
    memset(buff_message, 0, sizeof(buff_message));

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(buff_message, MSG_LENGTH, message, arg_ptr);
    va_end(arg_ptr);

    char out_message[MSG_LENGTH+1];
    sprintf(out_message, "%s%s\n", level_strings[level], buff_message);

    if (is_error) {
        platform_console_write_error(out_message, level);
    } else {
        platform_console_write(out_message, level);
    }
}
