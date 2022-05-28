#include "Instance.h"

#include <vector>

namespace efreet::engine::renderer::vulkan::instance {

b8 create(Context& context, const char* applicationName) {
    VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    {
        applicationInfo.apiVersion = VK_API_VERSION_1_3;
        applicationInfo.pApplicationName = applicationName;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Efreet Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    }

    std::vector<const char*> requiredExtensions;
    {
        requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

        #if defined(EFREET_PLATFORM_WINDOWS)
            requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        #elif defined(EFREET_PLATFORM_LINUX)
            requiredExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
        #elif defined(EFREET_PLATFORM_APPLE)
            requiredExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        #else
            #error "this platform id not supported by renderer yet"
        #endif

        DEBUG_OP({
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utilities

            E_DEBUG("Required extensions:");
            for (const auto& requiredExtension: requiredExtensions) {
                E_DEBUG("\t %s", requiredExtension);
            }
        });
    }

    std::vector<const char*> requiredValidationLayers;
    {
        DEBUG_OP({
            requiredValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
            // NOTE: enable this when needed for debugging.
            // requiredValidationLayers.push_back("VK_LAYER_LUNARG_api_dump");

            u32 availableLayerCount = 0;
            VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
            std::vector<VkLayerProperties> availableLayers(availableLayerCount);
            VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

            for (const auto& requiredValidationLayer: requiredValidationLayers) {
                E_DEBUG("%s layer is required", requiredValidationLayer);
                b32 found = false;
                for (u32 j = 0; j < availableLayerCount; ++j) {
                    if (::strcmp(requiredValidationLayer, availableLayers[j].layerName) == 0) { // TODO: strcmp
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    E_FATAL("Required validation layer is missing: %s", requiredValidationLayer);
                    return false;
                }
            }
        });
    }

    VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    {
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
        instanceCreateInfo.enabledLayerCount = requiredValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    }

    VK_CHECK(vkCreateInstance(&instanceCreateInfo, context.allocator, &context.instance));

    return context.instance != nullptr;
}

void destroy(Context& context) {
    vkDestroyInstance(context.instance, context.allocator);
    context.instance = nullptr;
    #ifdef EFREET_PLATFORM_WINDOWS
        DEBUG_OP(if (::GetLastError() == 6) { ::SetLastError(0); });
        E_ASSERT_DEBUG(::GetLastError() == 0);
    #endif
}

} // namespace efreet::engine::renderer::vulkan::instance