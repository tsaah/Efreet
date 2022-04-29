#pragma once

#include <basic_types.h>
#include <renderer_export.h>
#include <RendererBackendConfig.h>

namespace efreet::engine::renderer {

class RENDERER_EXPORT IRendererBackend {
public:
    virtual ~IRendererBackend() = default;

    virtual const char* name() = 0;
    virtual const char* description() = 0;
    virtual u32 version() = 0;

    virtual b8 init(const RendererBackendConfig& config) = 0;
    virtual void cleanup() = 0;
};

} // namespace efreet::engine::renderer