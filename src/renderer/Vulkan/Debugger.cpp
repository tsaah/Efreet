#include "Debugger.h"

namespace efreet::engine::renderer::vulkan::debugger {

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            E_ERROR("(vkdbg) %s", callbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            E_WARN("(vkdbg) %s", callbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            E_INFO("(vkdbg) %s", callbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            E_TRACE("(vkdbg) %s", callbackData->pMessage);
        } break;
        default: break;
    }
    return VK_FALSE;
}

} // namespace

b8 create(Context& context) {
    b8 result{ true };
    DEBUG_OP({
        const auto logSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        {
            debugUtilsMessengerCreateInfo.messageSeverity = logSeverity;
            debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugUtilsMessengerCreateInfo.pfnUserCallback = vkDebugCallback;
        }

        auto debugFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT"));
        if (debugFunction == nullptr) {
            E_ERROR("Failed to create debug messenger!");
            return false;
        }

        VK_CHECK(debugFunction(context.instance, &debugUtilsMessengerCreateInfo, context.allocator, &context.debugMessenger));
        result = context.debugMessenger != nullptr;
    });
    return result;
}

void destroy(Context& context) {
    DEBUG_OP({
        if (context.debugMessenger) {
            auto debugFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT"));
            debugFunction(context.instance, context.debugMessenger, context.allocator);
            context.debugMessenger = nullptr;
        }
    });
}

} // namespace efreet::engine::renderer::vulkan::debugger