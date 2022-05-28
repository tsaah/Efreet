#pragma once

#include "VulkanCommon.h"
#include "Context.h"

namespace efreet::engine::renderer::vulkan::debugger {

b8 create(Context& context);
void destroy(Context& context);

} // namespace efreet::engine::renderer::vulkan::debugger