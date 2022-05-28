#pragma once

#include "VulkanCommon.h"
#include "Context.h"

namespace efreet::engine::renderer::vulkan::instance {

b8 create(Context& context, const char* applicationName);
void destroy(Context& context);

} // namespace efreet::engine::renderer::vulkan::instance