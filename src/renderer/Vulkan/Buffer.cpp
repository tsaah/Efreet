#include "Buffer.h"
#include "Context.h"
#include "CommandBuffer.h"

namespace efreet::engine::renderer::vulkan::buffer {

b8 create(Context& context, u64 size, VkBufferUsageFlagBits usage, u32 memoryPropertyFlags,
    b8 bindOnCreate, b8 useFreelist, Buffer& outBuffer) {
    outBuffer.hasFreelist = useFreelist;
    outBuffer.totalSize = size;
    outBuffer.usage = usage;
    outBuffer.memoryPropertyFlags = memoryPropertyFlags;

    // if (useFreelist) {
    //     // Create a new freelist, if used.
    //     outBuffer.freelistMemoryRequirement = 0;
    //     freelist_create(size, &outBuffer.freelist_memory_requirement, 0, 0);
    //     outBuffer.freelist_block = kallocate(outBuffer.freelist_memory_requirement, MEMORY_TAG_RENDERER);
    //     freelist_create(size, &outBuffer.freelist_memory_requirement, outBuffer.freelist_block, &outBuffer.buffer_freelist);
    // }

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

    VK_CHECK(vkCreateBuffer(context.device.logicalDevice, &bufferInfo, context.allocator, &outBuffer.handle));

    // Gather memory requirements.
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context.device.logicalDevice, outBuffer.handle, &requirements);
    outBuffer.memoryIndex = context.findMemoryIndex(requirements.memoryTypeBits, outBuffer.memoryPropertyFlags);
    if (outBuffer.memoryIndex == -1) {
        E_ERROR("Unable to create vulkan buffer because the required memory type index was not found.");

        // Make sure to destroy the freelist.
        // cleanup_freelist(outBuffer);
        return false;
    }

    // Allocate memory info
    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)outBuffer.memoryIndex;

    // Allocate the memory.
    VkResult result = vkAllocateMemory(
        context.device.logicalDevice,
        &allocateInfo,
        context.allocator,
        &outBuffer.memory);

    if (result != VK_SUCCESS) {
        E_ERROR("Unable to create vulkan buffer because the required memory allocation failed. Error: %i", result);

        // Make sure to destroy the freelist.
        // cleanup_freelist(outBuffer);
        return false;
    }

    if (bindOnCreate) {
        bind(context, outBuffer, 0);
    }

    return true;
}

void destroy(Context& context, Buffer& buffer) {
    // if (buffer.freelistBlock) {
    //     // Make sure to destroy the freelist.
    //     cleanup_freelist(buffer);
    // }
    if (buffer.memory) {
        vkFreeMemory(context.device.logicalDevice, buffer.memory, context.allocator);
        buffer.memory = 0;
    }
    if (buffer.handle) {
        vkDestroyBuffer(context.device.logicalDevice, buffer.handle, context.allocator);
        buffer.handle = 0;
    }
    buffer.totalSize = 0;
    // buffer.usage = 0;
    buffer.isLocked = false;
}

b8 resize(Context& context, u64 newSize, Buffer& buffer, VkQueue queue, VkCommandPool pool) {
    // Sanity check.
    if (newSize < buffer.totalSize) {
        E_ERROR("vulkan_buffer_resize requires that new size be larger than the old. Not doing this could lead to data loss.");
        return false;
    }

    // if (buffer.has_freelist) {
    //     // Resize the freelist first, if used.
    //     u64 new_memory_requirement = 0;
    //     freelist_resize(&buffer.buffer_freelist, &new_memory_requirement, 0, 0, 0);
    //     void* new_block = kallocate(new_memory_requirement, MEMORY_TAG_RENDERER);
    //     void* old_block = 0;
    //     if (!freelist_resize(&buffer.buffer_freelist, &new_memory_requirement, new_block, newSize, &old_block)) {
    //         E_ERROR("vulkan_buffer_resize failed to resize internal free list.");
    //         kfree(new_block, new_memory_requirement, MEMORY_TAG_RENDERER);
    //         return false;
    //     }

    //     // Clean up the old memory, then assign the new properties over.
    //     kfree(old_block, buffer.freelist_memory_requirement, MEMORY_TAG_RENDERER);
    //     buffer.freelist_memory_requirement = new_memory_requirement;
    //     buffer.freelist_block = new_block;
    // }

    buffer.totalSize = newSize;

    // Create new buffer.
    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = newSize;
    bufferInfo.usage = buffer.usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

    VkBuffer new_buffer;
    VK_CHECK(vkCreateBuffer(context.device.logicalDevice, &bufferInfo, context.allocator, &new_buffer));

    // Gather memory requirements.
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context.device.logicalDevice, new_buffer, &requirements);

    // Allocate memory info
    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)buffer.memoryIndex;

    // Allocate the memory.
    VkDeviceMemory new_memory;
    VkResult result = vkAllocateMemory(context.device.logicalDevice, &allocateInfo, context.allocator, &new_memory);
    if (result != VK_SUCCESS) {
        E_ERROR("Unable to resize vulkan buffer because the required memory allocation failed. Error: %i", result);
        return false;
    }

    // Bind the new buffer's memory
    VK_CHECK(vkBindBufferMemory(context.device.logicalDevice, new_buffer, new_memory, 0));

    // Copy over the data
    copyTo(context, pool, 0, queue, buffer.handle, 0, new_buffer, 0, buffer.totalSize);

    // Make sure anything potentially using these is finished.
    vkDeviceWaitIdle(context.device.logicalDevice);

    // Destroy the old
    if (buffer.memory) {
        vkFreeMemory(context.device.logicalDevice, buffer.memory, context.allocator);
        buffer.memory = 0;
    }
    if (buffer.handle) {
        vkDestroyBuffer(context.device.logicalDevice, buffer.handle, context.allocator);
        buffer.handle = 0;
    }

    // Set new properties
    buffer.totalSize = newSize;
    buffer.memory = new_memory;
    buffer.handle = new_buffer;

    return true;
}

void bind(Context& context, Buffer& buffer, u64 offset) {
    VK_CHECK(vkBindBufferMemory(context.device.logicalDevice, buffer.handle, buffer.memory, offset));
}

void* lockMemory(Context& context, Buffer& buffer, u64 offset, u64 size, u32 flags) {
    void* data;
    VK_CHECK(vkMapMemory(context.device.logicalDevice, buffer.memory, offset, size, flags, &data));
    return data;
}

void unlockMemory(Context& context, Buffer& buffer) {
    vkUnmapMemory(context.device.logicalDevice, buffer.memory);
}

// b8 allocate(Buffer& buffer, u64 size, u64* outOffset) {
//     if (!size || !outOffset) {
//         E_ERROR("vulkan_buffer_allocate requires valid buffer, a nonzero size and valid pointer to hold offset.");
//         return false;
//     }

//     if (!buffer.hasFreelist) {
//         E_WARN("vulkan_buffer_allocate called on a buffer not using freelists. Offset will not be valid. Call vulkan_buffer_load_data instead.");
//         *outOffset = 0;
//         return true;
//     }
//     return freelist_allocate_block(&buffer.bufferFreelist, size, outOffset);
// }

// b8 free(Buffer& buffer, u64 size, u64 offset) {
//     if (!buffer || !size) {
//         E_ERROR("vulkan_buffer_free requires valid buffer and a nonzero size.");
//         return false;
//     }

//     if (!buffer.hasFreelist) {
//         E_WARN("vulkan_buffer_allocate called on a buffer not using freelists. Nothing was done.");
//         return true;
//     }
//     return freelist_free_block(&buffer.bufferFreelist, size, offset);
// }

void loadData(Context& context, Buffer& buffer, u64 offset, u64 size, u32 flags, const void* data) {
    void* dataPtr;
    VK_CHECK(vkMapMemory(context.device.logicalDevice, buffer.memory, offset, size, flags, &dataPtr));
    std::memcpy(dataPtr, data, size);
    vkUnmapMemory(context.device.logicalDevice, buffer.memory);
}

void copyTo(Context& context, VkCommandPool pool, VkFence fence, VkQueue queue, VkBuffer source,
    u64 sourceOffset, VkBuffer dest, u64 destOffset, u64 size) {
    vkQueueWaitIdle(queue);
    // Create a one-time-use command buffer.
    CommandBuffer tempCommandBuffer;
    commandBuffer::allocateAndBeginSingleUse(context, pool, tempCommandBuffer);

    // Prepare the copy command and add it to the command buffer.
    VkBufferCopy copy_region;
    copy_region.srcOffset = sourceOffset;
    copy_region.dstOffset = destOffset;
    copy_region.size = size;

    vkCmdCopyBuffer(tempCommandBuffer.handle, source, dest, 1, &copy_region);

    // Submit the buffer for execution and wait for it to complete.
    commandBuffer::endSingleUse(context, pool, tempCommandBuffer, queue);
}

} // namespace efreet::engine::renderer::vulkan::buffer