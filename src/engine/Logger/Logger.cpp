#include "Logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace efreet::engine {

const char* Logger::LOG_LEVEL_STRINGS[6] = { "F", "E", "W", "I", "D", "T" };
const platform::console::Color Logger::LOG_LEVEL_COLORS[6] = {
    platform::console::Color::COLOR_FATAL,
    platform::console::Color::COLOR_ERROR,
    platform::console::Color::COLOR_WARN,
    platform::console::Color::COLOR_INFO,
    platform::console::Color::COLOR_DEBUG,
    platform::console::Color::COLOR_TRACE,
};

Logger& Logger::instance() {
    static Logger i;
    return i;
}

void Logger::log(LogLevel level, const char* prefix, const char* message, ...) {
    const b32 isError = level < LogLevel::LVL_WARN;

    // TODO: better log message composing
    const i32 messageLength = 32000;

    char outBuffer[messageLength + 1];
    ::memset(outBuffer, 0, sizeof(outBuffer)); // TODO: memset

    va_list args;
    va_start(args, message);
    ::vsnprintf(outBuffer, 32000, message, args); // TODO: vsnprintf
    va_end(args);

    char outBuffer2[messageLength];
    if (prefix != nullptr) {
        ::sprintf(outBuffer2, "[%s] %s: %s\n", prefix, LOG_LEVEL_STRINGS[static_cast<u8>(level)], outBuffer); // TODO: sprintf
    } else {
        ::sprintf(outBuffer2, "%s: %s\n", LOG_LEVEL_STRINGS[static_cast<u8>(level)], outBuffer); // TODO: sprintf
    }

    if (isError) {
        platform::console::writeError(outBuffer2, LOG_LEVEL_COLORS[static_cast<u8>(level)]);
    } else {
        platform::console::write(outBuffer2, LOG_LEVEL_COLORS[static_cast<u8>(level)]);
    }
}

void Logger::reportAssertionFailure(const char* expression, const char* prefix, const char* message, const char* file, i32 line) {
    log(LogLevel::LVL_FATAL, prefix, "Assertion failure: %s, message: %s, in file: %s, line: %d", expression, message, file, line);
}

} // namespace efreet::engine
