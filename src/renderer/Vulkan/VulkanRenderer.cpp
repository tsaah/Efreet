#include "VulkanRenderer.h"

#include "VulkanCommon.h"
#include "Instance.h"
#include "Debugger.h"
#include "Surface.h"
#include "Device.h"
#include "Renderpass.h"
#include "CommandBuffer.h"

#include <Vector3D.h>

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

        // VK_CHECK(vkCreateFramebuffer(context.device.logicalDevice, &sc_framebuffer_create_info, context.allocator, &context.swapchain.framebuffers[i]));
        ++i;
    }
}

void createCommandBuffers() {
    const auto imageCount = context.swapchain.renderImages.size();
    if (context.graphicsCommandBuffers.empty()) {
        context.graphicsCommandBuffers.resize(imageCount);
    }

    for (u32 i = 0; i < imageCount; ++i) {
        if (context.graphicsCommandBuffers[i].handle) {
            commandBuffer::deallocate(context, context.device.graphicsCommandPool, context.graphicsCommandBuffers[i]);
        }
        commandBuffer::allocate(context, context.device.graphicsCommandPool, true, context.graphicsCommandBuffers[i]);
    }

    E_DEBUG("Vulkan command buffers created.");
}

b8 createBuffers() {
    VkMemoryPropertyFlagBits memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    // Geometry vertex buffer
    const u64 vertexBufferSize = sizeof(v3) * 1024 * 1024;
    if (!buffer::create(
            context,
            vertexBufferSize,
            static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
            memoryPropertyFlags,
            true,
            true,
            context.objectVertexBuffer)) {
        E_ERROR("Error creating vertex buffer.");
        return false;
    }

    // Geometry index buffer
    const u64 indexBufferSize = sizeof(u32) * 1024 * 1024;
    if (!buffer::create(
            context,
            indexBufferSize,
            static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
            memoryPropertyFlags,
            true,
            true,
            context.objectIndexBuffer)) {
        E_ERROR("Error creating vertex buffer.");
        return false;
    }

    return true;
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

    context.framebufferWidth = config.width;
    context.framebufferHeight = config.height;

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

    // Create command buffers.
    createCommandBuffers();

    // Create sync objects.
    context.imageAvailableSemaphores.resize(context.swapchain.maxFramesInFlight);
    context.queueCompleteSemaphores.resize(context.swapchain.maxFramesInFlight);

    for (u8 i = 0; i < context.swapchain.maxFramesInFlight; ++i) {
        VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(context.device.logicalDevice, &semaphoreCreateInfo, context.allocator, &context.imageAvailableSemaphores[i]);
        vkCreateSemaphore(context.device.logicalDevice, &semaphoreCreateInfo, context.allocator, &context.queueCompleteSemaphores[i]);

        // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered until a frame is "rendered" before it.
        VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(context.device.logicalDevice, &fenceCreateInfo, context.allocator, &context.inFlightFences[i]));
    }

    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
    // by this list.
    for (u32 i = 0; i < context.swapchain.renderImages.size(); ++i) {
        context.imagesInFlight[i] = 0;
    }

    createBuffers();

    // // Mark all geometries as invalid
    // for (u32 i = 0; i < VULKAN_MAX_GEOMETRY_COUNT; ++i) {
    //     context.geometries[i].id = INVALID_ID;
    // }

    E_INFO("Vulkan renderer initialized successfully.");
    return true;
}

void cleanup() {
    vkDeviceWaitIdle(context.device.logicalDevice);

    // Destroy in the opposite order of creation.
    // Destroy buffers
    buffer::destroy(context, context.objectVertexBuffer);
    buffer::destroy(context, context.objectIndexBuffer);

    // Sync objects
    for (u8 i = 0; i < context.swapchain.maxFramesInFlight; ++i) {
        if (context.imageAvailableSemaphores[i]) {
            vkDestroySemaphore(
                context.device.logicalDevice,
                context.imageAvailableSemaphores[i],
                context.allocator);
            context.imageAvailableSemaphores[i] = 0;
        }
        if (context.queueCompleteSemaphores[i]) {
            vkDestroySemaphore(
                context.device.logicalDevice,
                context.queueCompleteSemaphores[i],
                context.allocator);
            context.queueCompleteSemaphores[i] = 0;
        }
        vkDestroyFence(context.device.logicalDevice, context.inFlightFences[i], context.allocator);
    }
    context.imageAvailableSemaphores.clear();
    context.queueCompleteSemaphores.clear();

    // Command buffers
    for (u32 i = 0; i < context.swapchain.renderImages.size(); ++i) {
        if (context.graphicsCommandBuffers[i].handle) {
            commandBuffer::deallocate(context, context.device.graphicsCommandPool, context.graphicsCommandBuffers[i]);
            context.graphicsCommandBuffers[i].handle = nullptr;
        }
    }
    context.graphicsCommandBuffers.clear();

    // Destroy framebuffers
    for (u32 i = 0; i < context.swapchain.renderImages.size(); ++i) {
        vkDestroyFramebuffer(context.device.logicalDevice, context.worldFramebuffers[i], context.allocator);
        vkDestroyFramebuffer(context.device.logicalDevice, context.swapchain.framebuffers[i], context.allocator);
    }

    // Renderpasses
    // vulkan_renderpass_destroy(&context, &context.ui_renderpass);
    renderpass::destroy(context, context.mainRenderpass);

    swapchain::destroy(context, context.swapchain);
    device::destroy(context);
    surface::destroy(context);
    debugger::destroy(context);
    instance::destroy(context);
}

} // namespace efreet::engine::renderer::vulkan