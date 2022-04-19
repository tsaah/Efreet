#pragma once

#include "ILogger.h"

namespace efreet {

class LoggerProvider final {
public:
    inline static ILogger* logger() {
        return logger_;
    }

    inline static void setLogger(ILogger* logger) {
        logger_ = logger;
    }

private:
    inline static ILogger* logger_{ nullptr };
};

} // namespace efreet:


#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if EFREET_RELEASE == 1
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif

#define E_REPORT_ASSERT(expr) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->reportAssertionFailure(expr, "", __FILE__, __LINE__); }
#define E_REPORT_ASSERT_EXT(expr, message) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->reportAssertionFailure(expr, message, __FILE__, __LINE__); }

#define E_FATAL(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_FATAL, message, ##__VA_ARGS__); }

#ifndef E_ERROR
    #define E_ERROR(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_ERROR, message, ##__VA_ARGS__); }
#endif

#if LOG_WARN_ENABLED == 1
    #define E_WARN(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_WARN, message, ##__VA_ARGS__); }
#else
    #define E_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
    #define E_INFO(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_INFO, message, ##__VA_ARGS__); }
#else
    #define E_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define E_DEBUG(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_DEBUG, message, ##__VA_ARGS__); }
#else
    #define E_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
    #define E_TRACE(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_TRACE, message, ##__VA_ARGS__); }
#else
    #define E_TRACE(message, ...)
#endif
