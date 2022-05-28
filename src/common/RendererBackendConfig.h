#pragma once

#include "platform_detection.h"
#include "basic_types.h"
#include "RendererBackendType.h"
#include "ILogger.h"

namespace efreet {

struct RendererBackendConfig {
    engine::renderer::BackendType backendType{ engine::renderer::BackendType::None };
    ILogger* logger{ nullptr };
    const char* applicationName{ nullptr };
    u32 width{ 0 };
    u32 height{ 0 };
    #if defined(EFREET_PLATFORM_WINDOWS)
        ::HINSTANCE instance{ nullptr };
        ::HWND hwnd{ nullptr };
    #else
        #error "this platform id not supported by renderer yet"
    #endif
};

} // namespace efreet