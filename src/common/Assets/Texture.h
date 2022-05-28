#pragma once

#include <common.h>

namespace efreet {

struct Texture {
    enum class TextureFlag: u8 {
        HasTransparency = 0x1,
        IsWritable = 0x2,
        IsWrapped = 0x4,
    };
    using TextureFlagBits = u8;
    u32 id{ 0 };
    u32 width{ 0 };
    u32 height{ 0 };
    u8 channelCount{ 0 };
    TextureFlagBits flags{ 0 };
    u32 generation{ 0 };
    u8* data{ nullptr };
};

} // namespace efreet