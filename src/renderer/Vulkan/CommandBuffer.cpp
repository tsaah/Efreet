#include "CommandBuffer.h"
#include "Context.h"

namespace efreet::engine::renderer::vulkan::commandBuffer {

void allocate(Context& context, VkCommandPool pool, b8 isPrimary, CommandBuffer* outCommandBuffer) {

    outCommandBuffer = {};

    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.commandPool = pool;
    allocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.pNext = 0;

    outCommandBuffer->state = CommandBuffer::State::NotAllocated;
    VK_CHECK(vkAllocateCommandBuffers(context.device.logicalDevice, &allocateInfo, &outCommandBuffer->handle));
    outCommandBuffer->state = CommandBuffer::State::Ready;
}

void deallocate(Context& context, VkCommandPool pool, CommandBuffer* commandBuffer) {
    vkFreeCommandBuffers(context.device.logicalDevice, pool, 1, &commandBuffer->handle);
    commandBuffer->handle = nullptr;
    commandBuffer->state = CommandBuffer::State::NotAllocated;
}

void begin(CommandBuffer* commandBuffer, b8 isSingleUse, b8 isRenderpassContinue, b8 isSimultaneousUse) {
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = 0;
    if (isSingleUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (isRenderpassContinue) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (isSimultaneousUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_CHECK(vkBeginCommandBuffer(commandBuffer->handle, &beginInfo));
    commandBuffer->state = CommandBuffer::State::Recording;
}

void end(CommandBuffer* commandBuffer) {
    VK_CHECK(vkEndCommandBuffer(commandBuffer->handle));
    commandBuffer->state = CommandBuffer::State::RecordingEnded;
}

void updateSubmitted(CommandBuffer* commandBuffer) {
    commandBuffer->state = CommandBuffer::State::Submitted;
}

void reset(CommandBuffer* commandBuffer) {
    commandBuffer->state = CommandBuffer::State::Ready;
}

void allocateAndBeginSingleUse(Context& context, VkCommandPool pool, CommandBuffer* outCommandBuffer) {
    allocate(context, pool, true, outCommandBuffer);
    begin(outCommandBuffer, true, false, false);
}

void endSingleUse(Context& context, VkCommandPool pool, CommandBuffer* commandBuffer, VkQueue queue) {
    end(commandBuffer);

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, 0));

    // Wait for it to finish
    VK_CHECK(vkQueueWaitIdle(queue));

    // Free the command buffer.
    deallocate(context, pool, commandBuffer);
}

} // namespace efreet::engine::renderer::vulkan::commandBuffer