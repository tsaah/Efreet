#pragma once

#include "LoggerProvider.h"

#define EFREET_ASSERTIONS_ENABLED

#ifdef EFREET_ASSERTIONS_ENABLED
    #if _MSC_VER
        #include <intrin.h>
        #define debugBreak() __debugbreak()
    #else
        #define debugBreak() __builtin_trap()
    #endif

    #define E_PURE_ASSERT(expr) \
        { \
            if (expr) {} else { \
                debugBreak(); \
            } \
        }

    #define E_ASSERT(expr) \
        { \
            if (expr) {} else { \
                E_REPORT_ASSERT(#expr); \
                debugBreak(); \
            } \
        }

    #define E_ASSERT_EXT(expr, message) \
        { \
            if (expr) {} else { \
                E_REPORT_ASSERT_EXT(#expr, message); \
                debugBreak(); \
            } \
        }

    #ifdef _DEBUG
        #define E_PURE_ASSERT_DEBUG(expr) \
        { \
            if (expr) {} else { \
                debugBreak(); \
            } \
        }

        #define E_ASSERT_DEBUG(expr) \
        { \
            if (expr) {} else { \
                E_REPORT_ASSERT(#expr); \
                debugBreak(); \
            } \
        }

        #define E_ASSERT_DEBUG_EXT(expr, message)  \
        { \
            if (expr) {} else { \
                E_REPORT_ASSERT_EXT(#expr, message); \
                debugBreak(); \
            } \
        }

    #else
        #define E_PURE_ASSERT_DEBUG(expr)
        #define E_ASSERT_DEBUG(expr)
        #define E_ASSERT_DEBUG_EXT(expr, message)
    #endif
#else
    #define E_PURE_ASSERT(expr)
    #define E_ASSERT(expr)
    #define E_ASSERT_EXT(expr, message)
    #define E_ASSERT_DEBUG(expr)
    #define E_ASSERT_DEBUG_EXT(expr, message)
    #define E_PURE_ASSERT_DEBUG(expr)
#endif


