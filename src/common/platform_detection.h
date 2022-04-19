#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define EFREET_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #ifndef _WIN64
        #error "64-bit is required on Windows!"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    #define EFREET_PLATFORM_LINUX
    #if defined(__ANDROID__)
        #define EFREET_PLATFORM_ANDROID
    #endif
#elif defined(__unix__)
    #define EFREET_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
    #define EFREET_PLATFORM_POSIX
#elif __APPLE__
    #define EFREET_PLATFORM_APPLE
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #define EFREET_PLATFORM_IOS
        #define EFREET_PLATFORM_IOS_SIMULATOR
    #elif TARGET_OS_IPHONE
        #define EFREET_PLATFORM_IOS
    #elif TARGET_OS_MAC
    #else
        #error "Unknown Apple platform"
    #endif
#else
    #error "Unknown platform"
#endif
