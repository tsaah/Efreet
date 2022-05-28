#include "Renderpass.h"
#include "Context.h"
#include "CommandBuffer.h"

namespace efreet::engine::renderer::vulkan::renderpass {

void create(Context& context, Renderpass& outRenderpass, v4 renderArea, v4 clearColour,
    f32 depth, u32 stencil, u8 clearFlags, b8 hasPreviousPass, b8 hasNextPass) {

    outRenderpass.clearFlags = clearFlags;
    outRenderpass.renderArea = renderArea;
    outRenderpass.clearColour = clearColour;
    outRenderpass.hasPreviousPass = hasPreviousPass;
    outRenderpass.hasNextPass = hasNextPass;

    outRenderpass.depth = depth;
    outRenderpass.stencil = stencil;

    // Main subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    u32 attachmentADescriptionCount = 0;
    VkAttachmentDescription attachmentDescriptions[2];

    // Color attachment
    b8 doClearColour = (outRenderpass.clearFlags & static_cast<u8>(Renderpass::ClearFlag::ColorBuffer)) != 0;
    VkAttachmentDescription colorAttachment;
    colorAttachment.format = context.swapchain.imageFormat.format;  // TODO: configurable
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = doClearColour ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // If coming from a previous pass, should already be VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL. Otherwise undefined.
    colorAttachment.initialLayout = hasPreviousPass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

    // If going to another pass, use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL. Otherwise VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.
    colorAttachment.finalLayout = hasNextPass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
    colorAttachment.flags = 0;

    attachmentDescriptions[attachmentADescriptionCount] = colorAttachment;
    attachmentADescriptionCount++;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;  // Attachment description array index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    // Depth attachment, if there is one
    b8 doClearDepth = (outRenderpass.clearFlags & static_cast<u8>(Renderpass::ClearFlag::DepthBuffer)) != 0;
    if (doClearDepth) {
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = context.device.depthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = doClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachmentDescriptions[attachmentADescriptionCount] = depthAttachment;
        attachmentADescriptionCount++;

        // Depth attachment reference
        VkAttachmentReference depthAttachmentReference;
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // TODO: other attachment types (input, resolve, preserve)

        // Depth stencil data.
        subpass.pDepthStencilAttachment = &depthAttachmentReference;
    } else {
        attachmentDescriptions[attachmentADescriptionCount] = {};
        // kzero_memory(&attachmentDescriptions[attachmentADescriptionCount], sizeof(VkAttachmentDescription));
        subpass.pDepthStencilAttachment = 0;
    }

    // Input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    // Attachments used for multisampling colour attachments
    subpass.pResolveAttachments = 0;

    // Attachments not used in this subpass, but must be preserved for the next.
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    // Render pass dependencies. TODO: make this configurable.
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    // Render pass create.
    VkRenderPassCreateInfo renderpassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderpassCreateInfo.attachmentCount = attachmentADescriptionCount;
    renderpassCreateInfo.pAttachments = attachmentDescriptions;
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = &subpass;
    renderpassCreateInfo.dependencyCount = 1;
    renderpassCreateInfo.pDependencies = &dependency;
    renderpassCreateInfo.pNext = 0;
    renderpassCreateInfo.flags = 0;

    VK_CHECK(vkCreateRenderPass(context.device.logicalDevice, &renderpassCreateInfo, context.allocator, &outRenderpass.handle));
}

void destroy(Context& context, Renderpass& renderpass) {
    // if (renderpass && renderpass.handle) {
    if (renderpass.handle) {
        vkDestroyRenderPass(context.device.logicalDevice, renderpass.handle, context.allocator);
        renderpass.handle = nullptr;
    }
}

void begin(CommandBuffer& commandBuffer, Renderpass& renderpass, VkFramebuffer frameBuffer) {
    VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    beginInfo.renderPass = renderpass.handle;
    beginInfo.framebuffer = frameBuffer;
    beginInfo.renderArea.offset.x = renderpass.renderArea.x;
    beginInfo.renderArea.offset.y = renderpass.renderArea.y;
    beginInfo.renderArea.extent.width = renderpass.renderArea.z;
    beginInfo.renderArea.extent.height = renderpass.renderArea.w;

    beginInfo.clearValueCount = 0;
    beginInfo.pClearValues = 0;

    std::vector<VkClearValue> clearValues(2);
    // kzero_memory(clearValues, sizeof(VkClearValue) * 2);
    b8 doClearColour = (renderpass.clearFlags & static_cast<u8>(Renderpass::ClearFlag::ColorBuffer)) != 0;
    if (doClearColour) {
        // kcopy_memory(clearValues[beginInfo.clearValueCount].color.float32, renderpass.clearColour.elements, sizeof(f32) * 4);
        std::memcpy(clearValues[beginInfo.clearValueCount].color.float32, renderpass.clearColour.data, sizeof(f32) * 4);
        beginInfo.clearValueCount++;
    }

    b8 doClearDepth = (renderpass.clearFlags & static_cast<u8>(Renderpass::ClearFlag::DepthBuffer)) != 0;
    if (doClearDepth) {
        // kcopy_memory(clearValues[beginInfo.clearValueCount].color.float32, renderpass.clearColour.elements, sizeof(f32) * 4);
        std::memcpy(clearValues[beginInfo.clearValueCount].color.float32, renderpass.clearColour.data, sizeof(f32) * 4);
        clearValues[beginInfo.clearValueCount].depthStencil.depth = renderpass.depth;

        b8 doClearStencil = (renderpass.clearFlags & static_cast<u8>(Renderpass::ClearFlag::StencilBuffer)) != 0;
        clearValues[beginInfo.clearValueCount].depthStencil.stencil = doClearStencil ? renderpass.stencil : 0;
        beginInfo.clearValueCount++;
    }

    beginInfo.pClearValues = beginInfo.clearValueCount > 0 ? clearValues.data() : 0;

    vkCmdBeginRenderPass(commandBuffer.handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    commandBuffer.state = CommandBuffer::State::InRenderPass;
}

void end(CommandBuffer& commandBuffer, Renderpass& renderpass) {
    vkCmdEndRenderPass(commandBuffer.handle);
    commandBuffer.state = CommandBuffer::State::Recording;
}

} // namespace efreet::engine::renderer::vulkan::renderpass