#pragma once

#include "VulkanCommon.h"
#include <Vector4D.h>

namespace efreet::engine::renderer::vulkan {

struct Context;
struct CommandBuffer;

struct Renderpass {
    enum class ClearFlag: u8 {
        None = 0x0,
        ColorBuffer = 0x1,
        DepthBuffer = 0x2,
        StencilBuffer = 0x4
    };
    enum class State: u8 {
        Ready,
        Recording,
        InRenderPass,
        RecordingEnded,
        Submitted,
        NotAllocated
    };
    VkRenderPass handle{ nullptr };
    v4 renderArea;
    v4 clearColour;

    f32 depth{ 0 };
    u32 stencil{ 0 };

    u8 clearFlags{ 0 };
    b8 hasPreviousPass;
    b8 hasNextPass;

    State state{ State::NotAllocated };
};

namespace renderpass {

void create(Context& context, Renderpass& outRenderpass, v4 renderArea, v4 clearColour,
    f32 depth, u32 stencil, u8 clearFlags, b8 hasPreviousPass, b8 hasNextPass);
void destroy(Context& context, Renderpass& renderpass);
void begin(CommandBuffer& commandBuffer, Renderpass& renderpass, VkFramebuffer frameBuffer);
void end(CommandBuffer& commandBuffer, Renderpass& renderpass);

} // namespace renderpass

} // namespace efreet::engine::renderer::vulkan