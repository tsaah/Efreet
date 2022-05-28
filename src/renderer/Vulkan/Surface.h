#pragma once

#include "VulkanCommon.h"
#include "Context.h"

#include <RendererBackendConfig.h>

namespace efreet::engine::renderer::vulkan::surface {

b8 create(Context& context, const RendererBackendConfig& config);
void destroy(Context& context);

} // namespace efreet::engine::renderer::vulkan::surface