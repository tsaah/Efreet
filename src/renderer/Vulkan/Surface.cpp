#include "Surface.h"

#include <vector>

namespace efreet::engine::renderer::vulkan::surface {

b8 create(Context& context, const RendererBackendConfig& config) {
    if (config.hwnd == nullptr) {
        E_ERROR("while creating surface Window handle was nullptr.");
        return false;
    }
    // if (!state_ptr) {
    //     return false;
    // }

    VkResult result{ VK_SUCCESS };

    #if defined(EFREET_PLATFORM_WINDOWS)
        VkWin32SurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        createInfo.hinstance = config.instance;
        createInfo.hwnd = config.hwnd;

        result = vkCreateWin32SurfaceKHR(context.instance, &createInfo, context.allocator, &context.surface);
    // #elif defined(EFREET_PLATFORM_LINUX)
    //     VkXcbSurfaceCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR };
    //     createInfo.connection = state_ptr->connection;
    //     createInfo.window = state_ptr->window;

    //     result = vkCreateMetalSurfaceEXT(context.instance, &createInfo, context.allocator, &state_ptr->surface);
    // #elif defined(EFREET_PLATFORM_APPLE)
    //     VkMetalSurfaceCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT };
    //     createInfo.pLayer = state_ptr->layer;
    //     result = vkCreateMetalSurfaceEXT(context.instance, &createInfo, context.allocator, &state_ptr->surface);
    #else
        #error "this platform id not supported by renderer yet"
    #endif

    if (result != VK_SUCCESS) {
        E_ERROR("Vulkan surface creation failed.");
        return false;
    }

    // context.surface = state_ptr->surface;B

    return context.surface != nullptr;
}

void destroy(Context& context) {
    if (context.surface != nullptr) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = nullptr;
    }
}

} // namespace efreet::engine::renderer::vulkan::surface