#pragma once

#include "VulkanCommon.h"

#include <common.h>

#include <vector>

namespace efreet::engine::renderer::vulkan::device {

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

} // namespace efreet::engine::renderer::vulkan::device