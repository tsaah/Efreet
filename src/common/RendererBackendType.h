#pragma once

#include <basic_types.h>

namespace efreet::engine::renderer {

enum class BackendType: u8 {
    None,
    Dummy,
    OpenGL,
    Vulkan,
    DirectX
};

} // namespace efreet::engine::renderer
