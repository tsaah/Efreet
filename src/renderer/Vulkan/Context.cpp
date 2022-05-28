#include "Context.h"

namespace efreet::engine::renderer::vulkan {

i32 Context::findMemoryIndex(u32 typeFilter, u32 propertyFlags) const {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(device.physicalDevice, &memoryProperties);

    for (u32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    E_WARN("Unable to find suitable memory type!");
    return -1;
}

} // namespace efreet::engine::renderer::vulkan