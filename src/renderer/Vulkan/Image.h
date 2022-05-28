#pragma once

#include "VulkanCommon.h"


namespace efreet::engine::renderer::vulkan {

struct Context;
struct CommandBuffer;

struct Image {
    VkImage handle{ nullptr };
    VkDeviceMemory memory{ nullptr };
    VkImageView view{ nullptr };
    u32 width{ 0 };
    u32 height{ 0 };
};

namespace image {

b8 create(Context& context, VkImageType imageType,
    u32 width, u32 height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags,
    b32 createView, VkImageAspectFlags viewAspectFlags,
    Image* outImage);
void viewCreate(Context& context, VkFormat format, Image* image, VkImageAspectFlags aspectFlags);
void transitionLayout(Context& context, CommandBuffer* commandBuffer, Image* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void copyFromBuffer(Context& context, Image* image, VkBuffer buffer, CommandBuffer* commandBuffer);
void destroy(Context& context, Image* outImage);

} // namespace image

} // namespace efreet::engine::renderer::vulkan