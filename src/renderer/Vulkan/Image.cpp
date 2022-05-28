#include "Image.h"
#include "Context.h"
#include "CommandBuffer.h"

namespace efreet::engine::renderer::vulkan::image {

b8 create(Context& context, VkImageType imageType,
    u32 width, u32 height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags,
    b32 createView, VkImageAspectFlags viewAspectFlags,
    Image* outImage) {

    // Copy params
    outImage->width = width;
    outImage->height = height;

    // Creation info.
    VkImageCreateInfo imageCreateOnfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    {
        imageCreateOnfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateOnfo.extent.width = width;
        imageCreateOnfo.extent.height = height;
        imageCreateOnfo.extent.depth = 1;  // TODO: Support configurable depth.
        imageCreateOnfo.mipLevels = 4;     // TODO: Support mip mapping
        imageCreateOnfo.arrayLayers = 1;   // TODO: Support number of layers in the image.
        imageCreateOnfo.format = format;
        imageCreateOnfo.tiling = tiling;
        imageCreateOnfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateOnfo.usage = usage;
        imageCreateOnfo.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable sample count.
        imageCreateOnfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: Configurable sharing mode.
    }

    VK_CHECK(vkCreateImage(context.device.logicalDevice, &imageCreateOnfo, context.allocator, &outImage->handle));

    // Query memory requirements.
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(context.device.logicalDevice, outImage->handle, &memoryRequirements);

    i32 memoryType = context.findMemoryIndex(memoryRequirements.memoryTypeBits, memoryFlags);
    if (memoryType == -1) {
        E_ERROR("Required memory type not found. Image not valid.");
    }

    // Allocate memory
    VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = memoryType;
    VK_CHECK(vkAllocateMemory(context.device.logicalDevice, &memoryAllocateInfo, context.allocator, &outImage->memory));

    // Bind the memory
    VK_CHECK(vkBindImageMemory(context.device.logicalDevice, outImage->handle, outImage->memory, 0));  // TODO: configurable memory offset.

    // Create view
    if (createView) {
        outImage->view = 0;
        viewCreate(context, format, outImage, viewAspectFlags);
    }

    return true;
}

void viewCreate(Context& context, VkFormat format, Image* image, VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewCreateInfo.image = image->handle;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

    // TODO: Make configurable
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context.device.logicalDevice, &viewCreateInfo, context.allocator, &image->view));
}

void transitionLayout(Context& context, CommandBuffer* commandBuffer, Image* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = context.device.graphicsQueueIndex;
    barrier.dstQueueFamilyIndex = context.device.graphicsQueueIndex;
    barrier.image = image->handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destStage;

    // Don't care about the old layout - transition to optimal layout (for the underlying implementation).
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Don't care what stage the pipeline is in at the start.
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        // Used for copying
        destStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Transitioning from a transfer destination layout to a shader-readonly layout.
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // From a copying stage to...
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        // The fragment stage.
        destStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        E_FATAL("unsupported layout transition!");
        return;
    }

    vkCmdPipelineBarrier(commandBuffer->handle, sourceStage, destStage, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

void copyFromBuffer(Context& context, Image* image, VkBuffer buffer, CommandBuffer* commandBuffer) {
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageExtent.width = image->width;
    region.imageExtent.height = image->height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(commandBuffer->handle, buffer, image->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void destroy(Context& context, Image* outImage) {
    if (outImage->view != nullptr) {
        vkDestroyImageView(context.device.logicalDevice, outImage->view, context.allocator);
        outImage->view = nullptr;
    }
    if (outImage->memory != nullptr) {
        vkFreeMemory(context.device.logicalDevice, outImage->memory, context.allocator);
        outImage->memory = nullptr;
    }
    if (outImage->handle != nullptr) {
        vkDestroyImage(context.device.logicalDevice, outImage->handle, context.allocator);
        outImage->handle = nullptr;
    }
}

} // namespace efreet::engine::renderer::vulkan::image