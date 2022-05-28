#pragma once

#include "VulkanCommon.h"
#include "Image.h"

// #include <Assets/Texture.h>

#include <vector>

namespace efreet::engine::renderer::vulkan {

struct Context;

struct Swapchain {
    VkSurfaceFormatKHR imageFormat{};
    u8 maxFramesInFlight{ 0 } ;
    VkSwapchainKHR handle{ nullptr };
    // std::vector<Texture> renderTextures;
    Image depthAttachment{};
    std::vector<Image> renderImages;
    VkFramebuffer framebuffers[3];
};

namespace swapchain {

b8 create(Context& context, u32 width, u32 height, Swapchain& outSwapchain);
void destroy(Context& context, Swapchain& outSwapchain);

} // namespace swapchain

} // namespace efreet::engine::renderer::vulkan