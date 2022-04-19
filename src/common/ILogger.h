#pragma once

#include "basic_types.h"
#include "LogLevel.h"

namespace efreet {

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const char* message, ...) = 0;
    virtual void reportAssertionFailure(const char* expression, const char* message, const char* file, i32 line) = 0;
};

} // namespace efreet