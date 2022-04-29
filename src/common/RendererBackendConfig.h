#pragma once

#include <platform_detection.h>
#include <basic_types.h>
#include <ILogger.h>

namespace efreet {

struct RendererBackendConfig {
    ILogger* logger{ nullptr };
    const char* applicationName{ nullptr };
    #if defined(EFREET_PLATFORM_WINDOWS)
        ::HINSTANCE instance{ nullptr };
        ::HWND hwnd{ nullptr };
    #else
        #error "this platform id not supported by renderer yet"
    #endif
};

} // namespace efreet