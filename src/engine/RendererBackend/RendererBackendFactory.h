#pragma once

namespace efreet::engine::renderer {

class IRendererBackend;

class RendererBackendFactory final {
public:
    enum Type {
        DirectX,
        OpenGL,
        Vulkan
    };
    static IRendererBackend* create(Type rendererBackendType);
};

} // namespace efreet::engine::renderer
