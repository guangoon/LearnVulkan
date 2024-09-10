#define GLFW_INCLUDE_VULKAN
#include "hello_triangle.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <iostream>

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

bool HelloTriangle::CreateFramebuffers() {
  const std::vector<ImageParameters>& swap_chain_images = GetSwapChain().Images;
  framebuffers_.resize(swap_chain_images.size());

  for (size_t i = 0; i < swap_chain_images.size(); ++i) {
    VkFramebufferCreateInfo framebuffer_create_info = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType sType
        nullptr,       // const void                    *pNext
        0,             // VkFramebufferCreateFlags       flags
        render_pass_,  // VkRenderPass                   renderPass
        1,             // uint32_t                       attachmentCount
        &swap_chain_images[i].View,  // const VkImageView *pAttachments
        800,                         // uint32_t                       width
        600,                         // uint32_t                       height
        1                            // uint32_t                       layers
    };

    if (vkCreateFramebuffer(GetDevice(), &framebuffer_create_info, nullptr,
                            &framebuffers_[i]) != VK_SUCCESS) {
      std::cout << "Could not create a framebuffer!" << std::endl;
      return false;
    }
  }
  return true;
}

bool HelloTriangle::CreatePipeline() {
  Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
      vertex_shader_module =
          CreateShaderModule("data/2.1.hello_triangle/shader.vert.spv");
  Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
      fragment_shader_module =
          CreateShaderModule("data/2.1.hello_triangle/shader.frag.spv");

  if (!vertex_shader_module || !fragment_shader_module) {
    return false;
  }

  std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos = {
      // Vertex shader
      {
          VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // VkStructureType
                                                                // sType
          nullptr,  // const void                                    *pNext
          0,        // VkPipelineShaderStageCreateFlags               flags
          VK_SHADER_STAGE_VERTEX_BIT,  // VkShaderStageFlagBits stage
          vertex_shader_module.Get(),  // VkShaderModule module
          "main",  // const char                                    *pName
          nullptr  // const VkSpecializationInfo *pSpecializationInfo
      },
      // Fragment shader
      {
          VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // VkStructureType
                                                                // sType
          nullptr,  // const void                                    *pNext
          0,        // VkPipelineShaderStageCreateFlags               flags
          VK_SHADER_STAGE_FRAGMENT_BIT,  // VkShaderStageFlagBits stage
          fragment_shader_module.Get(),  // VkShaderModule module
          "main",  // const char                                    *pName
          nullptr  // const VkSpecializationInfo *pSpecializationInfo
      }};

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,  // VkStructureType
                                                                  // sType
      nullptr,  // const void                                    *pNext
      0,        // VkPipelineVertexInputStateCreateFlags          flags;
      0,        // uint32_t vertexBindingDescriptionCount
      nullptr,  // const VkVertexInputBindingDescription
                // *pVertexBindingDescriptions
      0,        // uint32_t vertexAttributeDescriptionCount
      nullptr   // const VkVertexInputAttributeDescription
                // *pVertexAttributeDescriptions
  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // VkStructureType
                                                                    // sType
      nullptr,  // const void                                    *pNext
      0,        // VkPipelineInputAssemblyStateCreateFlags        flags
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,  // VkPrimitiveTopology topology
      VK_FALSE                              // VkBool32 primitiveRestartEnable
  };

  VkViewport viewport = {
      0.0f,    // float                                          x
      0.0f,    // float                                          y
      800.0f,  // float                                          width
      600.0f,  // float                                          height
      0.0f,    // float                                          minDepth
      1.0f     // float                                          maxDepth
  };

  VkRect2D scissor = {{
                          // VkOffset2D offset
                          0,  // int32_t x
                          0  // int32_t                                        y
                      },
                      {
                          // VkExtent2D extent
                          800,  // int32_t width
                          600   // int32_t height
                      }};

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,  // VkStructureType
                                                              // sType
      nullptr,  // const void                                    *pNext
      0,        // VkPipelineViewportStateCreateFlags             flags
      1,        // uint32_t                                       viewportCount
      &viewport,  // const VkViewport                              *pViewports
      1,          // uint32_t                                       scissorCount
      &scissor    // const VkRect2D                                *pScissors
  };

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,  // VkStructureType
                                                                   // sType
      nullptr,   // const void                                    *pNext
      0,         // VkPipelineRasterizationStateCreateFlags        flags
      VK_FALSE,  // VkBool32 depthClampEnable
      VK_FALSE,  // VkBool32 rasterizerDiscardEnable
      VK_POLYGON_MODE_FILL,             // VkPolygonMode polygonMode
      VK_CULL_MODE_BACK_BIT,            // VkCullModeFlags cullMode
      VK_FRONT_FACE_COUNTER_CLOCKWISE,  // VkFrontFace frontFace
      VK_FALSE,                         // VkBool32 depthBiasEnable
      0.0f,                             // float depthBiasConstantFactor
      0.0f,  // float                                          depthBiasClamp
      0.0f,  // float depthBiasSlopeFactor
      1.0f   // float                                          lineWidth
  };

  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,  // VkStructureType
                                                                 // sType
      nullptr,  // const void                                    *pNext
      0,        // VkPipelineMultisampleStateCreateFlags          flags
      VK_SAMPLE_COUNT_1_BIT,  // VkSampleCountFlagBits rasterizationSamples
      VK_FALSE,               // VkBool32 sampleShadingEnable
      1.0f,  // float                                          minSampleShading
      nullptr,   // const VkSampleMask                            *pSampleMask
      VK_FALSE,  // VkBool32 alphaToCoverageEnable
      VK_FALSE   // VkBool32 alphaToOneEnable
  };

  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
      VK_FALSE,  // VkBool32                                       blendEnable
      VK_BLEND_FACTOR_ONE,   // VkBlendFactor srcColorBlendFactor
      VK_BLEND_FACTOR_ZERO,  // VkBlendFactor dstColorBlendFactor
      VK_BLEND_OP_ADD,       // VkBlendOp colorBlendOp
      VK_BLEND_FACTOR_ONE,   // VkBlendFactor srcAlphaBlendFactor
      VK_BLEND_FACTOR_ZERO,  // VkBlendFactor dstAlphaBlendFactor
      VK_BLEND_OP_ADD,       // VkBlendOp alphaBlendOp
      VK_COLOR_COMPONENT_R_BIT |
          VK_COLOR_COMPONENT_G_BIT |  // VkColorComponentFlags colorWriteMask
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,  // VkStructureType
                                                                 // sType
      nullptr,   // const void                                    *pNext
      0,         // VkPipelineColorBlendStateCreateFlags           flags
      VK_FALSE,  // VkBool32                                       logicOpEnable
      VK_LOGIC_OP_COPY,  // VkLogicOp logicOp
      1,  // uint32_t                                       attachmentCount
      &color_blend_attachment_state,  // const
                                      // VkPipelineColorBlendAttachmentState
                                      // *pAttachments
      {0.0f, 0.0f, 0.0f, 0.0f}        // float blendConstants[4]
  };

  Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
      pipeline_layout = CreatePipelineLayout();
  if (!pipeline_layout) {
    return false;
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info = {
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void                                    *pNext
      0,        // VkPipelineCreateFlags                          flags
      static_cast<uint32_t>(
          shader_stage_create_infos.size()),  // uint32_t stageCount
      shader_stage_create_infos
          .data(),  // const VkPipelineShaderStageCreateInfo         *pStages
      &vertex_input_state_create_info,  // const
                                        // VkPipelineVertexInputStateCreateInfo
                                        // *pVertexInputState;
      &input_assembly_state_create_info,  // const
                                          // VkPipelineInputAssemblyStateCreateInfo
                                          // *pInputAssemblyState
      nullptr,  // const VkPipelineTessellationStateCreateInfo
                // *pTessellationState
      &viewport_state_create_info,  // const VkPipelineViewportStateCreateInfo
                                    // *pViewportState
      &rasterization_state_create_info,  // const
                                         // VkPipelineRasterizationStateCreateInfo
                                         // *pRasterizationState
      &multisample_state_create_info,  // const
                                       // VkPipelineMultisampleStateCreateInfo
                                       // *pMultisampleState
      nullptr,  // const VkPipelineDepthStencilStateCreateInfo
                // *pDepthStencilState
      &color_blend_state_create_info,  // const
                                       // VkPipelineColorBlendStateCreateInfo
                                       // *pColorBlendState
      nullptr,  // const VkPipelineDynamicStateCreateInfo        *pDynamicState
      pipeline_layout.Get(),  // VkPipelineLayout layout
      render_pass_,           // VkRenderPass renderPass
      0,               // uint32_t                                       subpass
      VK_NULL_HANDLE,  // VkPipeline basePipelineHandle
      -1  // int32_t                                        basePipelineIndex
  };

  if (vkCreateGraphicsPipelines(GetDevice(), VK_NULL_HANDLE, 1,
                                &pipeline_create_info, nullptr,
                                &graphics_pipeline_) != VK_SUCCESS) {
    std::cout << "Could not create graphics pipeline!" << std::endl;
    return false;
  }
  return true;
}

Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
HelloTriangle::CreateShaderModule(const char* filename) {
  const std::vector<char> code = Tools::GetBinaryFileContents(filename);
  if (code.size() == 0) {
    return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
  }

  VkShaderModuleCreateInfo shader_module_create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,  // VkStructureType sType
      nullptr,      // const void                    *pNext
      0,            // VkShaderModuleCreateFlags      flags
      code.size(),  // size_t                         codeSize
      reinterpret_cast<const uint32_t*>(code.data())  // const uint32_t *pCode
  };

  VkShaderModule shader_module;
  if (vkCreateShaderModule(GetDevice(), &shader_module_create_info, nullptr,
                           &shader_module) != VK_SUCCESS) {
    std::cout << "Could not create shader module from a \"" << filename
              << "\" file!" << std::endl;
    return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>();
  }

  return Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>(
      shader_module, vkDestroyShaderModule, GetDevice());
}

Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
HelloTriangle::CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo layout_create_info = {
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void                    *pNext
      0,        // VkPipelineLayoutCreateFlags    flags
      0,        // uint32_t                       setLayoutCount
      nullptr,  // const VkDescriptorSetLayout   *pSetLayouts
      0,        // uint32_t                       pushConstantRangeCount
      nullptr   // const VkPushConstantRange     *pPushConstantRanges
  };

  VkPipelineLayout pipeline_layout;
  if (vkCreatePipelineLayout(GetDevice(), &layout_create_info, nullptr,
                             &pipeline_layout) != VK_SUCCESS) {
    std::cout << "Could not create pipeline layout!" << std::endl;
    return Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>();
  }

  return Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>(
      pipeline_layout, vkDestroyPipelineLayout, GetDevice());
}

bool HelloTriangle::CreateSemaphores() {
  VkSemaphoreCreateInfo semaphore_create_info = {
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void*              pNext
      0         // VkSemaphoreCreateFlags   flags
  };

  if ((vkCreateSemaphore(GetDevice(), &semaphore_create_info, nullptr,
                         &image_available_semaphore_) != VK_SUCCESS) ||
      (vkCreateSemaphore(GetDevice(), &semaphore_create_info, nullptr,
                         &rendering_finished_femaphore_) != VK_SUCCESS)) {
    std::cout << "Could not create semaphores!" << std::endl;
    return false;
  }

  return true;
}

bool HelloTriangle::CreateCommandBuffers() {
  if (!CreateCommandPool(GetGraphicsQueue().FamilyIndex,
                         &graphics_command_pool_)) {
    std::cout << "Could not create command pool!" << std::endl;
    return false;
  }

  uint32_t image_count = static_cast<uint32_t>(GetSwapChain().Images.size());
  graphics_command_buffers_.resize(image_count, VK_NULL_HANDLE);

  if (!AllocateCommandBuffers(graphics_command_pool_, image_count,
                              graphics_command_buffers_.data())) {
    std::cout << "Could not allocate command buffers!" << std::endl;
    return false;
  }
  return true;
}

bool HelloTriangle::CreateCommandPool(uint32_t queue_family_index,
                                      VkCommandPool* pool) {
  VkCommandPoolCreateInfo cmd_pool_create_info = {
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,  // VkStructureType sType
      nullptr,            // const void                    *pNext
      0,                  // VkCommandPoolCreateFlags       flags
      queue_family_index  // uint32_t                       queueFamilyIndex
  };

  if (vkCreateCommandPool(GetDevice(), &cmd_pool_create_info, nullptr, pool) !=
      VK_SUCCESS) {
    return false;
  }
  return true;
}

bool HelloTriangle::AllocateCommandBuffers(VkCommandPool pool, uint32_t count,
                                           VkCommandBuffer* command_buffers) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,  // VkStructureType sType
      nullptr,  // const void                    *pNext
      pool,     // VkCommandPool                  commandPool
      VK_COMMAND_BUFFER_LEVEL_PRIMARY,  // VkCommandBufferLevel           level
      count  // uint32_t                       bufferCount
  };

  if (vkAllocateCommandBuffers(GetDevice(), &command_buffer_allocate_info,
                               command_buffers) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool HelloTriangle::RecordCommandBuffers() {
  VkCommandBufferBeginInfo graphics_commandd_buffer_begin_info = {
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // VkStructureType sType
      nullptr,  // const void                            *pNext
      VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,  // VkCommandBufferUsageFlags
                                                     // flags
      nullptr  // const VkCommandBufferInheritanceInfo  *pInheritanceInfo
  };

  VkImageSubresourceRange image_subresource_range = {
      VK_IMAGE_ASPECT_COLOR_BIT,  // VkImageAspectFlags             aspectMask
      0,                          // uint32_t                       baseMipLevel
      1,                          // uint32_t                       levelCount
      0,  // uint32_t                       baseArrayLayer
      1   // uint32_t                       layerCount
  };

  VkClearValue clear_value = {
      {0.2f, 0.3f, 0.3f, 1.0f},  // VkClearColorValue              color
  };

  const std::vector<ImageParameters>& swap_chain_images = GetSwapChain().Images;

  for (size_t i = 0; i < graphics_command_buffers_.size(); ++i) {
    vkBeginCommandBuffer(graphics_command_buffers_[i],
                         &graphics_commandd_buffer_begin_info);

    if (GetPresentQueue().Handle != GetGraphicsQueue().Handle) {
      VkImageMemoryBarrier barrier_from_present_to_draw = {
          VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // VkStructureType sType
          nullptr,                    // const void                    *pNext
          VK_ACCESS_MEMORY_READ_BIT,  // VkAccessFlags srcAccessMask
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // VkAccessFlags dstAccessMask
          VK_IMAGE_LAYOUT_UNDEFINED,             // VkImageLayout oldLayout
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,       // VkImageLayout newLayout
          GetPresentQueue().FamilyIndex,         // uint32_t srcQueueFamilyIndex
          GetGraphicsQueue().FamilyIndex,        // uint32_t dstQueueFamilyIndex
          swap_chain_images[i].Handle,  // VkImage                        image
          image_subresource_range  // VkImageSubresourceRange subresourceRange
      };
      vkCmdPipelineBarrier(graphics_command_buffers_[i],
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
                           nullptr, 0, nullptr, 1,
                           &barrier_from_present_to_draw);
    }

    VkRenderPassBeginInfo render_pass_begin_info = {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,  // VkStructureType sType
        nullptr,           // const void                    *pNext
        render_pass_,      // VkRenderPass                   renderPass
        framebuffers_[i],  // VkFramebuffer                  framebuffer
        {                  // VkRect2D                       renderArea
         {
             // VkOffset2D                     offset
             0,  // int32_t                        x
             0   // int32_t                        y
         },
         {
             // VkExtent2D                     extent
             800,  // int32_t                        width
             600,  // int32_t                        height
         }},
        1,            // uint32_t                       clearValueCount
        &clear_value  // const VkClearValue            *pClearValues
    };

    vkCmdBeginRenderPass(graphics_command_buffers_[i], &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(graphics_command_buffers_[i],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    vkCmdDraw(graphics_command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(graphics_command_buffers_[i]);

    if (GetGraphicsQueue().Handle != GetPresentQueue().Handle) {
      VkImageMemoryBarrier barrier_from_draw_to_present = {
          VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // VkStructureType sType
          nullptr,  // const void                  *pNext
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // VkAccessFlags srcAccessMask
          VK_ACCESS_MEMORY_READ_BIT,             // VkAccessFlags dstAccessMask
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,       // VkImageLayout oldLayout
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,       // VkImageLayout newLayout
          GetGraphicsQueue().FamilyIndex,        // uint32_t srcQueueFamilyIndex
          GetPresentQueue().FamilyIndex,         // uint32_t dstQueueFamilyIndex
          swap_chain_images[i].Handle,  // VkImage                      image
          image_subresource_range  // VkImageSubresourceRange subresourceRange
      };
      vkCmdPipelineBarrier(graphics_command_buffers_[i],
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr,
                           0, nullptr, 1, &barrier_from_draw_to_present);
    }
    if (vkEndCommandBuffer(graphics_command_buffers_[i]) != VK_SUCCESS) {
      std::cout << "Could not record command buffer!" << std::endl;
      return false;
    }
  }
  return true;
}

void HelloTriangle::ChildClear() {
  if (GetDevice() != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(GetDevice());

    if ((graphics_command_buffers_.size() > 0) &&
        (graphics_command_buffers_[0] != VK_NULL_HANDLE)) {
      vkFreeCommandBuffers(
          GetDevice(), graphics_command_pool_,
          static_cast<uint32_t>(graphics_command_buffers_.size()),
          graphics_command_buffers_.data());
      graphics_command_buffers_.clear();
    }

    if (graphics_command_pool_ != VK_NULL_HANDLE) {
      vkDestroyCommandPool(GetDevice(), graphics_command_pool_, nullptr);
      graphics_command_pool_ = VK_NULL_HANDLE;
    }

    if (graphics_pipeline_ != VK_NULL_HANDLE) {
      vkDestroyPipeline(GetDevice(), graphics_pipeline_, nullptr);
      graphics_pipeline_ = VK_NULL_HANDLE;
    }

    if (render_pass_ != VK_NULL_HANDLE) {
      vkDestroyRenderPass(GetDevice(), render_pass_, nullptr);
      render_pass_ = VK_NULL_HANDLE;
    }

    for (size_t i = 0; i < framebuffers_.size(); ++i) {
      if (framebuffers_[i] != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(GetDevice(), framebuffers_[i], nullptr);
        framebuffers_[i] = VK_NULL_HANDLE;
      }
    }
    framebuffers_.clear();
  }
}

bool HelloTriangle::ChildOnWindowSizeChanged() {
  if (!CreateRenderPass()) {
    return false;
  }
  if (!CreateFramebuffers()) {
    return false;
  }
  if (!CreatePipeline()) {
    return false;
  }
  if (!CreateCommandBuffers()) {
    return false;
  }
  if (!RecordCommandBuffers()) {
    return false;
  }

  return true;
}

HelloTriangle::~HelloTriangle() {
  ChildClear();

  if (GetDevice() != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(GetDevice());

    if (image_available_semaphore_ != VK_NULL_HANDLE) {
      vkDestroySemaphore(GetDevice(), image_available_semaphore_, nullptr);
    }

    if (rendering_finished_femaphore_ != VK_NULL_HANDLE) {
      vkDestroySemaphore(GetDevice(), rendering_finished_femaphore_, nullptr);
    }
  }
}

HelloTriangle::HelloTriangle() {}

bool HelloTriangle::Draw() {
  VkSwapchainKHR swap_chain = GetSwapChain().Handle;
  uint32_t image_index;

  VkResult result = vkAcquireNextImageKHR(GetDevice(), swap_chain, UINT64_MAX,
                                          image_available_semaphore_,
                                          VK_NULL_HANDLE, &image_index);
  switch (result) {
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      return OnWindowSizeChanged();
    default:
      std::cout << "Problem occurred during swap chain image acquisition!"
                << std::endl;
      return false;
  }

  VkPipelineStageFlags wait_dst_stage_mask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info = {
      VK_STRUCTURE_TYPE_SUBMIT_INFO,  // VkStructureType              sType
      nullptr,                        // const void                  *pNext
      1,  // uint32_t                     waitSemaphoreCount
      &image_available_semaphore_,  // const VkSemaphore *pWaitSemaphores
      &wait_dst_stage_mask,  // const VkPipelineStageFlags  *pWaitDstStageMask;
      1,                     // uint32_t                     commandBufferCount
      &graphics_command_buffers_[image_index],  // const VkCommandBuffer
                                                // *pCommandBuffers
      1,  // uint32_t                     signalSemaphoreCount
      &rendering_finished_femaphore_  // const VkSemaphore *pSignalSemaphores
  };

  if (vkQueueSubmit(GetGraphicsQueue().Handle, 1, &submit_info,
                    VK_NULL_HANDLE) != VK_SUCCESS) {
    return false;
  }

  VkPresentInfoKHR present_info = {
      VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,  // VkStructureType              sType
      nullptr,                             // const void                  *pNext
      1,  // uint32_t                     waitSemaphoreCount
      &rendering_finished_femaphore_,  // const VkSemaphore *pWaitSemaphores
      1,             // uint32_t                     swapchainCount
      &swap_chain,   // const VkSwapchainKHR        *pSwapchains
      &image_index,  // const uint32_t              *pImageIndices
      nullptr        // VkResult                    *pResults
  };
  result = vkQueuePresentKHR(GetPresentQueue().Handle, &present_info);

  switch (result) {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
      return OnWindowSizeChanged();
    default:
      std::cout << "Problem occurred during image presentation!" << std::endl;
      return false;
  }
  return true;
}
