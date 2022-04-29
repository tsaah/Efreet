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

struct SwapchainSupportInfo final {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Device final {
    VkPhysicalDevice physicalDevice{ nullptr };
    VkDevice logicalDevice{ nullptr };
    SwapchainSupportInfo swapchainSupportInfo;

    i32 graphicsQueueIndex{ -1 };
    i32 presentQueueIndex{ -1 };
    i32 computeQueueIndex{ -1 };
    i32 transferQueueIndex{ -1 };
    b32 supportsDeviceLocalHostVisible{ false };

    VkQueue graphicsQueue{ nullptr };
    VkQueue presentQueue{ nullptr };
    VkQueue computeQueue{ nullptr };
    VkQueue transferQueue{ nullptr };

    VkCommandPool graphicsCommandPool{ nullptr };

    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceMemoryProperties memory{};

    VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
};

struct Context final{
    // f32 frame_delta_time;
    // u32 framebuffer_width;
    // u32 framebuffer_height;
    // /** @brief The generation of the framebuffer when it was last created. Set to framebuffer_size_generation when updated. */
    // u64 framebuffer_size_last_generation;

    VkInstance instance{ nullptr };
    VkAllocationCallbacks* allocator{ nullptr };
    VkSurfaceKHR surface{ nullptr };

    DEBUG_OP(
    VkDebugUtilsMessengerEXT debugMessenger{ nullptr };
    )

    Device device;

    // Swapchain swapchain;

    // Renderpass main_renderpass;
    // Renderpass ui_renderpass;

    // Buffer object_vertex_buffer;
    // Buffer object_index_buffer;

    // CommandBuffer* graphicsCommandBuffers{ nullptr };

    // /** @brief The semaphores used to indicate image availability, one per frame. @note: darray */
    // VkSemaphore* image_available_semaphores;

    // /** @brief The semaphores used to indicate queue availability, one per frame. @note: darray */
    // VkSemaphore* queue_complete_semaphores;

    // /** @brief The current number of in-flight fences. */
    // u32 in_flight_fence_count;
    // /** @brief The in-flight fences, used to indicate to the application when a frame is busy/ready. */
    // VkFence in_flight_fences[2];

    // /** @brief Holds pointers to fences which exist and are owned elsewhere, one per frame. */
    // VkFence images_in_flight[3];

    // /** @brief The current image index. */
    // u32 image_index;

    // /** @brief The current frame. */
    // u32 current_frame;

    // /** @brief Indicates if the swapchain is currently being recreated. */
    // b8 recreating_swapchain;

    // /** @brief The A collection of loaded geometries. @todo TODO: make dynamic */
    // vulkan_geometry_data geometries[VULKAN_MAX_GEOMETRY_COUNT];

    // /** @brief Framebuffers used for world rendering. @note One per frame. */
    // VkFramebuffer world_framebuffers[3];

    // /**
    //  * @brief A function pointer to find a memory index of the given type and with the given properties.
    //  * @param type_filter The types of memory to search for.
    //  * @param property_flags The required properties which must be present.
    //  * @returns The index of the found memory type. Returns -1 if not found.
    //  */
    // i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
};

} // namespace efreet::engine::renderer::vulkan