#pragma once

#include "ILogger.h"

namespace efreet {

class LoggerProvider final {
public:
    inline static ILogger* logger() {
        return logger_;
    }

    inline static const char* prefix() {
        return prefix_;
    }

    inline static void setLogger(ILogger* logger, const char* prefix) {
        logger_ = logger;
        prefix_ = prefix;
    }

private:
    inline static ILogger* logger_{ nullptr };
    inline static const char* prefix_{ nullptr };
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

#define E_REPORT_ASSERT(expr) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->reportAssertionFailure(expr, efreet::LoggerProvider::prefix(), "", __FILE__, __LINE__); }
#define E_REPORT_ASSERT_EXT(expr, message) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->reportAssertionFailure(expr, efreet::LoggerProvider::prefix(), message, __FILE__, __LINE__); }

#define E_FATAL(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_FATAL, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }

#ifndef E_ERROR
    #define E_ERROR(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_ERROR, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }
#endif

#if LOG_WARN_ENABLED == 1
    #define E_WARN(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_WARN, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }
#else
    #define E_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
    #define E_INFO(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_INFO, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }
#else
    #define E_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define E_DEBUG(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_DEBUG, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }
#else
    #define E_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
    #define E_TRACE(message, ...) if (efreet::LoggerProvider::logger() != nullptr) { efreet::LoggerProvider::logger()->log(efreet::LogLevel::LVL_TRACE, efreet::LoggerProvider::prefix(), message, ##__VA_ARGS__); }
#else
    #define E_TRACE(message, ...)
#endif
