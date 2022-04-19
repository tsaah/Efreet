#include "RendererBackendFactory.h"

#include <Vulkan/VulkanRenderer.h>

namespace efreet::engine::renderer {

IRendererBackend* RendererBackendFactory::create(Type rendererBackendType) {
    IRendererBackend* result{ nullptr };
    switch (rendererBackendType) {
        case Type::DirectX: {
            result = nullptr;
        } break;
        case Type::OpenGL: {
            result = nullptr;
        } break;
        case Type::Vulkan: {
            result = new vulkan::VulkanRenderer;
        } break;
        default: result = nullptr;
    }
    return result;
}

} // namespace efreet::engine::renderer
