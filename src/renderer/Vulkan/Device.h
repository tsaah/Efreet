#pragma once

#include "VulkanCommon.h"
#include "SwapchainSupportInfo.h"

#include <vector>

namespace efreet::engine::renderer::vulkan {
struct Context;

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

namespace device {

struct Requirements {
    b8 graphics{ true };
    b8 present{ true };
    b8 compute{ true };
    b8 transfer{ true };
    b8 samplerAnisotropy{ true };
    b8 discreteGpu{ true };
    std::vector<const char*> deviceExtensionNames;
};

b8 create(Context& context, const Requirements& requirements);
void destroy(Context& context);
b8 detectDepthFormat(Device& device);

} // namespace device

} // namespace efreet::engine::renderer::vulkan