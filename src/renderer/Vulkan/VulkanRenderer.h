#pragma once

#include <common.h>
#include <IRendererBackend.h>

namespace efreet::engine::renderer::vulkan {

class RENDERER_EXPORT VulkanRenderer final: public IRendererBackend {
public:
    const char* name() override;
    const char* description() override;
    u32 version() override;

    b32 init(const RendererBackendConfig& config) override;
    void cleanup() override;
};

} // namespace efreet::engine::renderer::vulkan