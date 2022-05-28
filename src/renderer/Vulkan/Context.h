#pragma once

#include "VulkanCommon.h"
#include "Device.h"
#include "Swapchain.h"
#include "Renderpass.h"

#include <vector>

namespace efreet::engine::renderer::vulkan {

struct Context final{
    // f32 frame_delta_time;
    u32 framebufferWidth{ 0 };
    u32 framebufferHeight{ 0 };
    // /** @brief The generation of the framebuffer when it was last created. Set to framebuffer_size_generation when updated. */
    // u64 framebuffer_size_last_generation;

    VkInstance instance{ nullptr };
    VkAllocationCallbacks* allocator{ nullptr };
    VkSurfaceKHR surface{ nullptr };

    DEBUG_OP(
    VkDebugUtilsMessengerEXT debugMessenger{ nullptr };
    )

    Device device;

    Swapchain swapchain;

    Renderpass mainRenderpass;
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
    u32 currentFrame;

    // /** @brief Indicates if the swapchain is currently being recreated. */
    // b8 recreating_swapchain;

    // /** @brief The A collection of loaded geometries. @todo TODO: make dynamic */
    // vulkan_geometry_data geometries[VULKAN_MAX_GEOMETRY_COUNT];

    /** @brief Framebuffers used for world rendering. @note One per frame. */
    VkFramebuffer worldFramebuffers[3];

    // /**
    //  * @brief A function pointer to find a memory index of the given type and with the given properties.
    //  * @param type_filter The types of memory to search for.
    //  * @param property_flags The required properties which must be present.
    //  * @returns The index of the found memory type. Returns -1 if not found.
    //  */
    // i32 (*find_memory_index)(u32 type_filter, u32 property_flags);


    i32 findMemoryIndex(u32 typeFilter, u32 propertyFlags) const;
};

} // namespace efreet::engine::renderer::vulkan