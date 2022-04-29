#include "VulkanRenderer.h"

#include "VulkanCommon.h"
#include "Device.h"

#undef _HAS_EXCEPTIONS
#include <vector>

namespace efreet::engine::renderer::vulkan {

namespace {


const char* rendererName = "Vulkan Renderer";
const char* rendererDescription = "A Vulkan Renderer";
i32 rendererVersion = 0;

Context context;

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

b8 createInstance(const char* applicationName) {
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

void destroyInstance() {
    vkDestroyInstance(context.instance, context.allocator);
    context.instance = nullptr;
    #ifdef EFREET_PLATFORM_WINDOWS
        DEBUG_OP(if (::GetLastError() == 6) { ::SetLastError(0); });
        E_ASSERT_DEBUG(::GetLastError() == 0);
    #endif
}

b8 createDebugger() {
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

void destroyDebugger() {
    DEBUG_OP({
        if (context.debugMessenger) {
            auto debugFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT"));
            debugFunction(context.instance, context.debugMessenger, context.allocator);
            context.debugMessenger = nullptr;
        }
    });
}

b8 createSurface(const RendererBackendConfig& config) {
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

        // result = vkCreateWin32SurfaceKHR(context.instance, &createInfo, context.allocator, &state_ptr->surface);
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

void destroySurface() {
    if (context.surface != nullptr) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = nullptr;
    }
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

b8 VulkanRenderer::init(const RendererBackendConfig& config) {
    LoggerProvider::setLogger(config.logger, "RENDERER");

    if (!createInstance(config.applicationName)) { return false; }
    if (!createDebugger()) { return false; }
    if (!createSurface(config)) { return false; }

    // Device creation
    // TODO: These requirements should probably be driven by engine and come from config
    device::Requirements requirements = {};
    requirements.graphics = true;
    requirements.present = true;
    requirements.transfer = true;
    requirements.compute = true;
    requirements.samplerAnisotropy = true;
#if KPLATFORM_APPLE
    requirements.discreteGpu = false;
#else
    requirements.discreteGpu = true;
#endif
    requirements.deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    if (!device::create(context, requirements)) { return false; }

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
//     vkDeviceWaitIdle(context.device.logical_device);

//     // Destroy in the opposite order of creation.
//     // Destroy buffers
//     vulkan_buffer_destroy(&context, &context.object_vertex_buffer);
//     vulkan_buffer_destroy(&context, &context.object_index_buffer);

//     // Sync objects
//     for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
//         if (context.image_available_semaphores[i]) {
//             vkDestroySemaphore(
//                 context.device.logical_device,
//                 context.image_available_semaphores[i],
//                 context.allocator);
//             context.image_available_semaphores[i] = 0;
//         }
//         if (context.queue_complete_semaphores[i]) {
//             vkDestroySemaphore(
//                 context.device.logical_device,
//                 context.queue_complete_semaphores[i],
//                 context.allocator);
//             context.queue_complete_semaphores[i] = 0;
//         }
//         vkDestroyFence(context.device.logical_device, context.in_flight_fences[i], context.allocator);
//     }
//     darray_destroy(context.image_available_semaphores);
//     context.image_available_semaphores = 0;

//     darray_destroy(context.queue_complete_semaphores);
//     context.queue_complete_semaphores = 0;

//     // Command buffers
//     for (u32 i = 0; i < context.swapchain.image_count; ++i) {
//         if (context.graphics_command_buffers[i].handle) {
//             vulkan_command_buffer_free(
//                 &context,
//                 context.device.graphics_command_pool,
//                 &context.graphics_command_buffers[i]);
//             context.graphics_command_buffers[i].handle = 0;
//         }
//     }
//     darray_destroy(context.graphics_command_buffers);
//     context.graphics_command_buffers = 0;

//     // Destroy framebuffers
//     for (u32 i = 0; i < context.swapchain.image_count; ++i) {
//         vkDestroyFramebuffer(context.device.logical_device, context.world_framebuffers[i], context.allocator);
//         vkDestroyFramebuffer(context.device.logical_device, context.swapchain.framebuffers[i], context.allocator);
//     }

//     // Renderpasses
//     vulkan_renderpass_destroy(&context, &context.ui_renderpass);
//     vulkan_renderpass_destroy(&context, &context.main_renderpass);

//     // Swapchain
//     vulkan_swapchain_destroy(&context, &context.swapchain);

    device::destroy(context);

    destroySurface();
    destroyDebugger();
    destroyInstance();
}

} // namespace efreet::engine::renderer::vulkan