#pragma once

#include <common.h>
#include <platform_detection.h>

#if defined(EFREET_PLATFORM_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(EFREET_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XCB_KHR
#elif defined(EFREET_PLATFORM_APPLE)
    #define VK_USE_PLATFORM_METAL_EXT
#else
#error "this platform id not supported by renderer yet"
#endif

#include <vulkan/vulkan.h>

#include <vector>

#define VK_CHECK(expr) E_ASSERT(expr == VK_SUCCESS)

namespace efreet::engine::renderer::vulkan {

} // namespace efreet::engine::renderer::vulkan