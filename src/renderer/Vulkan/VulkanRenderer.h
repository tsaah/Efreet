#pragma once

#include <common.h>
#include <renderer_export.h>
#include <RendererBackendType.h>
#include <RendererBackendConfig.h>

extern "C" {
namespace efreet::engine::renderer::vulkan {

RENDERER_EXPORT BackendType backendType();
RENDERER_EXPORT u32 version();

RENDERER_EXPORT b8 init(const RendererBackendConfig& config);
RENDERER_EXPORT void cleanup();

} // namespace efreet::engine::renderer::vulkan

} // extern "C"