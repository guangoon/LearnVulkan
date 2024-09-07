#define GLFW_INCLUDE_VULKAN
#include "hello_triangle.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <iostream>

#include "vulkan_common.h"

bool HelloTriangle::CreateRenderPass() {
  VkAttachmentDescription attachment_descriptions[] = {{
      0,                             // VkAttachmentDescriptionFlags   flags
      GetSwapChain().Format,         // VkFormat                       format
      VK_SAMPLE_COUNT_1_BIT,         // VkSampleCountFlagBits          samples
      VK_ATTACHMENT_LOAD_OP_CLEAR,   // VkAttachmentLoadOp             loadOp
      VK_ATTACHMENT_STORE_OP_STORE,  // VkAttachmentStoreOp            storeOp
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,   // VkAttachmentLoadOp stencilLoadOp
      VK_ATTACHMENT_STORE_OP_DONT_CARE,  // VkAttachmentStoreOp stencilStoreOp
      VK_IMAGE_LAYOUT_UNDEFINED,         // VkImageLayout initialLayout;
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR    // VkImageLayout finalLayout
  }};

  VkAttachmentReference color_attachment_references[] = {{
      0,  // uint32_t                       attachment
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout layout
  }};

  VkSubpassDescription subpass_descriptions[] = {{
      0,                                // VkSubpassDescriptionFlags      flags
      VK_PIPELINE_BIND_POINT_GRAPHICS,  // VkPipelineBindPoint pipelineBindPoint
      0,        // uint32_t                       inputAttachmentCount
      nullptr,  // const VkAttachmentReference   *pInputAttachments
      1,        // uint32_t                       colorAttachmentCount
      color_attachment_references,  // const VkAttachmentReference
                                    // *pColorAttachments
      nullptr,  // const VkAttachmentReference   *pResolveAttachments
      nullptr,  // const VkAttachmentReference   *pDepthStencilAttachment
      0,        // uint32_t                       preserveAttachmentCount
      nullptr   // const uint32_t*                pPreserveAttachments
  }};

  VkRenderPassCreateInfo render_pass_create_info = {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void                    *pNext
      0,        // VkRenderPassCreateFlags        flags
      1,        // uint32_t                       attachmentCount
      attachment_descriptions,  // const VkAttachmentDescription *pAttachments
      1,                        // uint32_t                       subpassCount
      subpass_descriptions,     // const VkSubpassDescription    *pSubpasses
      0,       // uint32_t                       dependencyCount
      nullptr  // const VkSubpassDependency     *pDependencies
  };

  if (vkCreateRenderPass(GetDevice(), &render_pass_create_info, nullptr,
                         &render_pass_) != VK_SUCCESS) {
    std::cout << "Could not create render pass!" << std::endl;
    return false;
  }

  return true;
}
