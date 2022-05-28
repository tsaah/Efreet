#pragma once

#include <common.h>

namespace efreet {

struct TextureSystem {


};

namespace textureSystem {

Texture* texture_system_wrap_internal(const char* name, u32 width, u32 height, u8 channel_count, b8 has_transparency, b8 is_writeable, b8 register_texture, void* internal_data);

void resize(Texture& texture, u32 width, u32 height, b8);

} // namespace textureSystem

} // namespace efreet