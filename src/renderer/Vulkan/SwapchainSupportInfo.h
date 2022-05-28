#pragma once

#include "VulkanCommon.h"

#include <vector>

namespace efreet::engine::renderer::vulkan {

struct SwapchainSupportInfo final {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

inline void querySwapchainSupport(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, SwapchainSupportInfo& swapchainSupportinfo) {
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportinfo.capabilities));

    u32 formatCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));
    if (formatCount != 0) {
        swapchainSupportinfo.formats.resize(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupportinfo.formats.data()));
    }

    u32 presentModeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    if (presentModeCount != 0) {
        swapchainSupportinfo.presentModes.resize(presentModeCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, swapchainSupportinfo.presentModes.data()));
    }
}

} // namespace efreet::engine::renderer::vulkan