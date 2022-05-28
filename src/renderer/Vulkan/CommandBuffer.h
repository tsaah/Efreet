#pragma once

#include "VulkanCommon.h"

namespace efreet::engine::renderer::vulkan {

struct Context;

struct CommandBuffer {
    enum class State: u8 {
        Ready,
        Recording,
        InRenderPass,
        RecordingEnded,
        Submitted,
        NotAllocated
    };
    VkCommandBuffer handle{ nullptr };
    State state{ State::NotAllocated };
};

namespace commandBuffer {

void allocate(Context& context, VkCommandPool pool, b8 isPrimary, CommandBuffer* outCommandBuffer);
void deallocate(Context& context, VkCommandPool pool, CommandBuffer* commandBuffer);
void begin(CommandBuffer* commandBuffer, b8 isSingleUse, b8 isRenderpassContinue, b8 isSimultaneousUse);
void end(CommandBuffer* commandBuffer);
void updateSubmitted(CommandBuffer* commandBuffer);
void reset(CommandBuffer* commandBuffer);
void allocateAndBeginSingleUse(Context& context, VkCommandPool pool, CommandBuffer* outCommandBuffer);
void endSingleUse(Context& context, VkCommandPool pool, CommandBuffer* commandBuffer, VkQueue queue);

} // namespace commandBuffer

} // namespace efreet::engine::renderer::vulkan