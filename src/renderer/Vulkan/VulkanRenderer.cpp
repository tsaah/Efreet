#include "VulkanRenderer.h"

#include "VulkanCommon.h"
#include "Instance.h"
#include "Debugger.h"
#include "Surface.h"
#include "Device.h"
#include "Renderpass.h"

#undef _HAS_EXCEPTIONS
#include <vector>

namespace efreet::engine::renderer::vulkan {

namespace {

i32 rendererVersion = 0;
Context context;

void regenerateFramebuffers() {
    u32 i = 0;
    for (auto& image: context.swapchain.renderImages) {
        VkImageView worldAttachments[2] = { image.view, context.swapchain.depthAttachment.view };
        VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
        framebuffer_create_info.renderPass = context.mainRenderpass.handle;
        framebuffer_create_info.attachmentCount = 2;
        framebuffer_create_info.pAttachments = worldAttachments;
        framebuffer_create_info.width = context.framebufferWidth;
        framebuffer_create_info.height = context.framebufferHeight;
        framebuffer_create_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(context.device.logicalDevice, &framebuffer_create_info, context.allocator, &context.worldFramebuffers[i]));

        // // Swapchain framebuffers (UI pass). Outputs to swapchain images
        // VkImageView uiAttachments[1] = { image.view };
        // VkFramebufferCreateInfo sc_framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        // sc_framebuffer_create_info.renderPass = context.uiRenderpass.handle;
        // sc_framebuffer_create_info.attachmentCount = 1;
        // sc_framebuffer_create_info.pAttachments = uiAttachments;
        // sc_framebuffer_create_info.width = context.framebufferWidth;
        // sc_framebuffer_create_info.height = context.framebufferHeight;
        // sc_framebuffer_create_info.layers = 1;

        // VK_CHECK(vkCreateFramebuffer(context.device.logical_device, &sc_framebuffer_create_info, context.allocator, &context.swapchain.framebuffers[i]));
        ++i;
    }
}

} // namespace

BackendType backendType() {
    return BackendType::Vulkan;
}

u32 version() {
    return rendererVersion;
}

b8 init(const RendererBackendConfig& config) {
    LoggerProvider::setLogger(config.logger, "RENDERER");

    if (!instance::create(context, config.applicationName)) { return false; }
    if (!debugger::create(context)) { return false; }
    if (!surface::create(context, config)) { return false; }

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
    if (!swapchain::create(context, config.width, config.height, context.swapchain)) { return false; }

//     // World render pass
    renderpass::create(context, context.mainRenderpass,
        { 0, 0, static_cast<f32>(context.framebufferWidth), static_cast<f32>(context.framebufferHeight) },
        { 0.0f, 0.0f, 0.2f, 1.0f },
        1.0f,
        0,
        static_cast<u8>(Renderpass::ClearFlag::ColorBuffer) | static_cast<u8>(Renderpass::ClearFlag::DepthBuffer) | static_cast<u8>(Renderpass::ClearFlag::StencilBuffer),
        false, true);

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

    regenerateFramebuffers();

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
//     for (u32 i = 0; i < context.swapchain.imageCount; ++i) {
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

void cleanup() {
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
//     for (u32 i = 0; i < context.swapchain.imageCount; ++i) {
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
//     for (u32 i = 0; i < context.swapchain.imageCount; ++i) {
//         vkDestroyFramebuffer(context.device.logical_device, context.world_framebuffers[i], context.allocator);
//         vkDestroyFramebuffer(context.device.logical_device, context.swapchain.framebuffers[i], context.allocator);
//     }

//     // Renderpasses
//     vulkan_renderpass_destroy(&context, &context.ui_renderpass);
    renderpass::destroy(context, context.mainRenderpass);

    swapchain::destroy(context, context.swapchain);
    device::destroy(context);
    surface::destroy(context);
    debugger::destroy(context);
    instance::destroy(context);
}

} // namespace efreet::engine::renderer::vulkan