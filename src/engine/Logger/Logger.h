#pragma once

#include "Console/Console.h"

#include <ILogger.h>

namespace efreet::engine {

class Logger final: public ILogger {
public:
    static Logger& instance();

    void log(LogLevel level, const char* prefix, const char* message, ...) override;
    void reportAssertionFailure(const char* expression, const char* prefix, const char* message, const char* file, i32 line) override;

private:
    Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    LogLevel logLevel_{ LogLevel::LVL_WARN };
    static const char* LOG_LEVEL_STRINGS[6];
    static const platform::console::Color LOG_LEVEL_COLORS[6];
};

} // namespace efreet::engine
