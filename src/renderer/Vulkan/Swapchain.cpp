#include "Swapchain.h"
#include "Context.h"

#include <maths.h>

namespace efreet::engine::renderer::vulkan::swapchain {

namespace {

b8 create_(Context& context, u32 width, u32 height, Swapchain& outSwapchain) {
    VkExtent2D swapchainExtent = { width, height };

    // Choose a swap surface format.
    b8 found = false;
    for (const auto& format: context.device.swapchainSupportInfo.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            outSwapchain.imageFormat = format;
            found = true;
            break;
        }
    }

    if (!found) {
        outSwapchain.imageFormat = context.device.swapchainSupportInfo.formats[0];
    }

    VkPresentModeKHR presentMode{ VK_PRESENT_MODE_FIFO_KHR };
    for (const auto& mode: context.device.swapchainSupportInfo.presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            break;
        }
    }

    // Requery swapchain support.
    querySwapchainSupport(context.device.physicalDevice, context.surface, context.device.swapchainSupportInfo);

    // Swapchain extent
    if (context.device.swapchainSupportInfo.capabilities.currentExtent.width != UINT32_MAX) {
        swapchainExtent = context.device.swapchainSupportInfo.capabilities.currentExtent;
    }

    // Clamp to the value allowed by the GPU.
    VkExtent2D min = context.device.swapchainSupportInfo.capabilities.minImageExtent;
    VkExtent2D max = context.device.swapchainSupportInfo.capabilities.maxImageExtent;
    swapchainExtent.width = clamp(swapchainExtent.width, min.width, max.width);
    swapchainExtent.height = clamp(swapchainExtent.height, min.height, max.height);

    u32 imageCount = context.device.swapchainSupportInfo.capabilities.minImageCount + 1;
    if (context.device.swapchainSupportInfo.capabilities.maxImageCount > 0 && imageCount > context.device.swapchainSupportInfo.capabilities.maxImageCount) {
        imageCount = context.device.swapchainSupportInfo.capabilities.maxImageCount;
    }

    outSwapchain.maxFramesInFlight = imageCount - 1;

    // Swapchain create info
    VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    {
        swapchainCreateInfo.surface = context.surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = outSwapchain.imageFormat.format;
        swapchainCreateInfo.imageColorSpace = outSwapchain.imageFormat.colorSpace;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    // Setup the queue family indices
    if (context.device.graphicsQueueIndex != context.device.presentQueueIndex) {
        u32 queueFamilyIndices[] = { static_cast<u32>(context.device.graphicsQueueIndex), static_cast<u32>(context.device.presentQueueIndex) };
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = 0;
    }

    swapchainCreateInfo.preTransform = context.device.swapchainSupportInfo.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(context.device.logicalDevice, &swapchainCreateInfo, context.allocator, &outSwapchain.handle));

    // Start with a zero frame index.
    context.currentFrame = 0;

    // Images
    u32 swapchainImageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context.device.logicalDevice, outSwapchain.handle, &swapchainImageCount, 0));
    // if (outSwapchain.renderTextures.empty()) {
    //     outSwapchain.renderTextures.reserve(swapchainImageCount);// = (texture**)kallocate(sizeof(texture*) * imageCount, MEMORY_TAG_RENDERER);
    //     // If creating the array, then the internal texture objects aren't created yet either.
    //     // for (u32 i = 0; i < imageCount; ++i) {
    //     for (auto& texture: outSwapchain.renderTextures) {
    //         // void* internalData = kallocate(sizeof(Image), MEMORY_TAG_TEXTURE);
    //         auto* data = reinterpret_cast<u8*>(::std::malloc(sizeof(Image)));

    //         char texName[38] = "__internal_vulkan_swapchain_image_0__";
    //         texName[34] = '0' + (char)outSwapchain.renderTextures.size();

    //         texture = texture_system_wrap_internal(
    //             texName,
    //             swapchainExtent.width,
    //             swapchainExtent.height,
    //             4,
    //             false,
    //             true,
    //             false,
    //             data);
    //     }
    // } else {
    //     for (auto& texture: outSwapchain.renderTextures) {
    //         // Just update the dimensions.
    //         texture_system_resize(texture, swapchainExtent.width, swapchainExtent.height, false);
    //     }
    // }
    std::vector<VkImage> swapchainImages(swapchainImageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(context.device.logicalDevice, outSwapchain.handle, &swapchainImageCount, swapchainImages.data()));
    outSwapchain.renderImages.resize(swapchainImageCount);
    for (u32 i = 0; i < swapchainImageCount; ++i) {
        // Update the internal image for each.
        // Image* image = (Image*)outSwapchain.renderTextures[i]->internalData;
        auto& image = outSwapchain.renderImages[i];
        image.handle = swapchainImages[i];
        image.width = swapchainExtent.width;
        image.height = swapchainExtent.height;
    }

    // Views
    for (u32 i = 0; i < swapchainImageCount; ++i) {
        // Image* image = (Image*)outSwapchain.renderTextures[i]->internalData;
        auto& image = outSwapchain.renderImages[i];

        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = image.handle;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = outSwapchain.imageFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context.device.logicalDevice, &viewInfo, context.allocator, &image.view));
    }

    // Depth resources
    if (!device::detectDepthFormat(context.device)) {
        context.device.depthFormat = VK_FORMAT_UNDEFINED;
        E_FATAL("Failed to find a supported format!");
    }

    // Create depth image and its view.
    image::create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchainExtent.width,
        swapchainExtent.height,
        context.device.depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &outSwapchain.depthAttachment);

    E_INFO("Swapchain created successfully.");
    return true;
}

} // namespace

b8 create(Context& context, u32 width, u32 height, Swapchain& outSwapchain) {
    return create_(context, width, height, outSwapchain);
}

void destroy(Context& context, Swapchain& outSwapchain) {
    vkDeviceWaitIdle(context.device.logicalDevice);
    image::destroy(context, &outSwapchain.depthAttachment);

    // for (auto& renderTexture: outSwapchain.renderTextures) {
    for (auto& image: outSwapchain.renderImages) {
        // auto* image = (Image*)renderTexture->internalData;
        vkDestroyImageView(context.device.logicalDevice, image.view, context.allocator);
    }

    vkDestroySwapchainKHR(context.device.logicalDevice, outSwapchain.handle, context.allocator);
}

} // namespace efreet::engine::renderer::vulkan::swapchain