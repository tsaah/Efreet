#pragma once

#include "VulkanCommon.h"

namespace efreet::engine::renderer::vulkan {

struct Context;

struct Buffer {
    u64 totalSize{ 0 };
    VkBuffer handle{ nullptr };
    VkBufferUsageFlagBits usage;
    b8 isLocked{ false };
    VkDeviceMemory memory{ nullptr };
    i32 memoryIndex{ 0 };
    u32 memoryPropertyFlags{ 0 };
    u64 freelistMemoryRequirement{ 0 };
    void* freelistBlock{ nullptr };
    // freelist buffer_freelist;
    b8 hasFreelist{ false };
};

namespace buffer {

b8 create(Context& context, u64 size, VkBufferUsageFlagBits usage, u32 memoryPropertyFlags,
    b8 bindOnCreate, b8 useFreelist, Buffer& outBuffer);

void destroy(Context& context, Buffer& buffer);

b8 resize(Context& context, u64 newSize, Buffer& buffer, VkQueue queue, VkCommandPool pool);

void bind(Context& context, Buffer& buffer, u64 offset);

void* lockMemory(Context& context, Buffer& buffer, u64 offset, u64 size, u32 flags);

void unlockMemory(Context& context, Buffer& buffer);

// b8 allocate(Buffer& buffer, u64 size, u64* outOffset);

// b8 free(Buffer& buffer, u64 size, u64 offset);

void loadData(Context& context, Buffer& buffer, u64 offset, u64 size, u32 flags, const void* data);

void copyTo(Context& context, VkCommandPool pool, VkFence fence, VkQueue queue, VkBuffer source,
    u64 sourceOffset, VkBuffer dest, u64 destOffset, u64 size);

} // namespace buffer

} // namespace efreet::engine::renderer::vulkan