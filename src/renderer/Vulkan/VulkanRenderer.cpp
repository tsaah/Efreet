#include "VulkanRenderer.h"

#include <vulkan/vulkan.h>

#undef _HAS_EXCEPTIONS
#include <vector>


#define VK_CHECK(expr)               \
    {                                \
        E_ASSERT(expr == VK_SUCCESS); \
    }

namespace efreet::engine::renderer::vulkan {

namespace {

const char* rendererName = "Vulkan Renderer";
const char* rendererDescription = "A Vulkan Renderer";
i32 rendererVersion = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    // switch (messageSeverity) {
    //     default:
    //     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    //         E_ERROR(callbackData->pMessage);
    //         break;
    //     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    //         E_WARN(callbackData->pMessage);
    //         break;
    //     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    //         E_INFO(callbackData->pMessage);
    //         break;
    //     case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    //         E_TRACE(callbackData->pMessage);
    //         break;
    // }
    return VK_FALSE;
}

} // namespace

const char* VulkanRenderer::name() {
    return rendererName;
}

const char* VulkanRenderer::description() {
    return rendererDescription;
}

u32 VulkanRenderer::version() {
    return rendererVersion;
}

b32 VulkanRenderer::init(const RendererBackendConfig& config) {
    LoggerProvider::setLogger(config.logger);

    // Setup Vulkan instance.
    VkApplicationInfo applicationInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    applicationInfo.apiVersion = VK_API_VERSION_1_3;
    applicationInfo.pApplicationName = config.applicationName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Efreet Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    // Obtain a list of required extensions
    std::vector<const char*> requiredExtensions;
    requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);  // Generic surface extension
//     getRequiredPlatformExtensionNames(requiredExtensions);       // Platform-specific extension(s)

#ifdef _DEBUG
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utilities

//     E_DEBUG("Required extensions:");
//     for (const auto& requiredExtension: requiredExtensions) {
//         E_DEBUG(requiredExtension);
//     }
#endif

    instanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Validation layers.
    std::vector<const char*> requiredValidationLayers;

// If validation should be done, get a list of the required validation layert names
// and make sure they exist. Validation layers should only be enabled on non-release builds.
#ifdef _DEBUG
//     E_INFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    requiredValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
    // NOTE: enable this when needed for debugging.
    // requiredValidationLayers.push_back("VK_LAYER_LUNARG_api_dump");

    // Obtain a list of available validation layers
    u32 availableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

    // Verify all required layers are available.
    // for (const auto& requiredValidationLayer: requiredValidationLayers) {
    // for (u32 i = 0; i < required_validation_layer_count; ++i) {
//         E_INFO("Searching for layer: %s...", requiredValidationLayer);
        // b32 found = false;
        // for (u32 j = 0; j < availableLayerCount; ++j) {
//             if (strings_equal(requiredValidationLayer, availableLayers[j].layerName)) {
                // found = true;
//                 E_INFO("Found.");
                // break;
//             }
        // }

        // if (!found) {
//             E_FATAL("Required validation layer is missing: %s", requiredValidationLayer);
            // return false;
        // }
    // }
//     E_INFO("All required validation layers are present.");
#endif

    instanceCreateInfo.enabledLayerCount = requiredValidationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    // VK_CHECK(vkCreateInstance(&instanceCreateInfo, context.allocator, &context.instance));
//     E_INFO("Vulkan Instance created.");

//     // Debugger
// #ifdef _DEBUG
//     E_DEBUG("Creating Vulkan debugger...");
//     const auto logSeverity =
//         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
//         // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

//     VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
//     debugUtilsMessengerCreateInfo.messageSeverity = logSeverity;
//     debugUtilsMessengerCreateInfo.messageType =
//         VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
//     debugUtilsMessengerCreateInfo.pfnUserCallback = vkDebugCallback;

//     auto debugFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT"));
//     E_ASSERT_MSG(debugFunction, "Failed to create debug messenger!");
//     VK_CHECK(func(context.instance, &debugUtilsMessengerCreateInfo, context.allocator, &context.debug_messenger));
//     E_DEBUG("Vulkan debugger created.");
// #endif

//     // Surface
//     E_DEBUG("Creating Vulkan surface...");
//     if (!platform_create_vulkan_surface(&context)) {
//         E_ERROR("Failed to create platform surface!");
//         return false;
//     }
//     E_DEBUG("Vulkan surface created.");

//     // Device creation
//     if (!vulkan_device_create(&context)) {
//         E_ERROR("Failed to create device!");
//         return false;
//     }

//     // Swapchain
//     vulkan_swapchain_create(
//         &context,
//         context.framebuffer_width,
//         context.framebuffer_height,
//         &context.swapchain);

//     // World render pass
//     vulkan_renderpass_create(
//         &context,
//         &context.main_renderpass,
//         (vec4){0, 0, context.framebuffer_width, context.framebuffer_height},
//         (vec4){0.0f, 0.0f, 0.2f, 1.0f},
//         1.0f,
//         0,
//         RENDERPASS_CLEAR_COLOUR_BUFFER_FLAG | RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG | RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG,
//         false, true);

//     // UI renderpass
//     vulkan_renderpass_create(
//         &context,
//         &context.ui_renderpass,
//         (vec4){0, 0, context.framebuffer_width, context.framebuffer_height},
//         (vec4){0.0f, 0.0f, 0.0f, 0.0f},
//         1.0f,
//         0,
//         RENDERPASS_CLEAR_NONE_FLAG,
//         true, false);

//     // Regenerate swapchain and world framebuffers
//     regenerate_framebuffers();

//     // Create command buffers.
//     create_command_buffers(backend);

//     // Create sync objects.
//     context.image_available_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
//     context.queue_complete_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);

//     for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
//         VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
//         vkCreateSemaphore(context.device.logical_device, &semaphore_create_info, context.allocator, &context.image_available_semaphores[i]);
//         vkCreateSemaphore(context.device.logical_device, &semaphore_create_info, context.allocator, &context.queue_complete_semaphores[i]);

//         // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
//         // This will prevent the application from waiting indefinitely for the first frame to render since it
//         // cannot be rendered until a frame is "rendered" before it.
//         VkFenceCreateInfo fence_create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
//         fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//         VK_CHECK(vkCreateFence(context.device.logical_device, &fence_create_info, context.allocator, &context.in_flight_fences[i]));
//     }

//     // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
//     // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
//     // by this list.
//     for (u32 i = 0; i < context.swapchain.image_count; ++i) {
//         context.images_in_flight[i] = 0;
//     }

//     create_buffers(&context);

//     // Mark all geometries as invalid
//     for (u32 i = 0; i < VULKAN_MAX_GEOMETRY_COUNT; ++i) {
//         context.geometries[i].id = INVALID_ID;
//     }

    E_INFO("Vulkan renderer initialized successfully.");
    return true;
}

void VulkanRenderer::cleanup() {

}

} // namespace efreet::engine::renderer::vulkan