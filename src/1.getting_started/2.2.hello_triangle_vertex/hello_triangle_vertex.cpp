#define GLFW_INCLUDE_VULKAN
#include "hello_triangle_vertex.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <iostream>

bool HelloTriangle::CreateRenderPass() {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = GetSwapChain().Format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_references{};
  color_attachment_references.attachment = 0;
  color_attachment_references.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_references;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(GetDevice(), &render_pass_info, nullptr,
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
    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass_;
    framebuffer_create_info.attachmentCount = 1;
    framebuffer_create_info.pAttachments = &swap_chain_images[i].View;
    framebuffer_create_info.width = 800;
    framebuffer_create_info.height = 600;
    framebuffer_create_info.layers = 1;

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
          CreateShaderModule("data/2.2.hello_triangle/shader.vert.spv");
  Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
      fragment_shader_module =
          CreateShaderModule("data/2.2.hello_triangle/shader.frag.spv");

  if (!vertex_shader_module || !fragment_shader_module) {
    return false;
  }

  std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
  VkPipelineShaderStageCreateInfo vertex_shader_module_create_info = {};
  vertex_shader_module_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_shader_module_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_module_create_info.module = vertex_shader_module.Get();
  vertex_shader_module_create_info.pName = "main";
  shader_stage_create_infos.push_back(vertex_shader_module_create_info);

  VkPipelineShaderStageCreateInfo fragment_shader_module_create_info = {};
  fragment_shader_module_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_shader_module_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  fragment_shader_module_create_info.module = fragment_shader_module.Get();
  fragment_shader_module_create_info.pName = "main";
  shader_stage_create_infos.push_back(fragment_shader_module_create_info);

  VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
  vertex_input_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, pos);

  vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
  vertex_input_state_create_info.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertex_input_state_create_info.pVertexBindingDescriptions =
      &bindingDescription;
  vertex_input_state_create_info.pVertexAttributeDescriptions =
      attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
  input_assembly_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_state_create_info.topology =
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f};

  VkRect2D scissor = {{0, 0}, {800, 600}};

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
  viewport_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_create_info.viewportCount = 1;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.scissorCount = 1;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
  rasterization_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_state_create_info.depthClampEnable = VK_FALSE;
  rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
  rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterization_state_create_info.lineWidth = 1.0f;
  rasterization_state_create_info.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
  multisample_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisample_state_create_info.minSampleShading = 1.0f;
  multisample_state_create_info.sampleShadingEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
      VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
  color_blend_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_state_create_info.attachmentCount = 1;
  color_blend_state_create_info.pAttachments = &color_blend_attachment_state;

  Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
      pipeline_layout = CreatePipelineLayout();
  if (!pipeline_layout) {
    return false;
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = shader_stage_create_infos.size();
  pipeline_create_info.pStages = shader_stage_create_infos.data();
  pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
  pipeline_create_info.pViewportState = &viewport_state_create_info;
  pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
  pipeline_create_info.pMultisampleState = &multisample_state_create_info;
  pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
  pipeline_create_info.layout = pipeline_layout.Get();
  pipeline_create_info.renderPass = render_pass_;

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

  VkShaderModuleCreateInfo shader_module_create_info = {};
  shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_create_info.codeSize = code.size();
  shader_module_create_info.pCode =
      reinterpret_cast<const uint32_t*>(code.data());

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
  VkPipelineLayoutCreateInfo layout_create_info = {};
  layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

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
  VkSemaphoreCreateInfo semaphore_create_info = {};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

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
  VkCommandPoolCreateInfo cmd_pool_create_info = {};
  cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmd_pool_create_info.queueFamilyIndex = queue_family_index;

  if (vkCreateCommandPool(GetDevice(), &cmd_pool_create_info, nullptr, pool) !=
      VK_SUCCESS) {
    return false;
  }
  return true;
}

bool HelloTriangle::AllocateCommandBuffers(VkCommandPool pool, uint32_t count,
                                           VkCommandBuffer* command_buffers) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
  command_buffer_allocate_info.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.commandPool = pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = count;

  if (vkAllocateCommandBuffers(GetDevice(), &command_buffer_allocate_info,
                               command_buffers) != VK_SUCCESS) {
    return false;
  }
  return true;
}

bool HelloTriangle::RecordCommandBuffers() {
  VkCommandBufferBeginInfo graphics_commandd_buffer_begin_info = {};
  graphics_commandd_buffer_begin_info.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  graphics_commandd_buffer_begin_info.flags =
      VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  VkImageSubresourceRange image_subresource_range = {};
  image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_subresource_range.levelCount = 1;
  image_subresource_range.layerCount = 1;

  VkClearValue clear_value = {{0.2f, 0.3f, 0.3f, 1.0f}};

  const std::vector<ImageParameters>& swap_chain_images = GetSwapChain().Images;

  for (size_t i = 0; i < graphics_command_buffers_.size(); ++i) {
    vkBeginCommandBuffer(graphics_command_buffers_[i],
                         &graphics_commandd_buffer_begin_info);

    if (GetPresentQueue().Handle != GetGraphicsQueue().Handle) {
      VkImageMemoryBarrier barrier_from_present_to_draw = {};
      barrier_from_present_to_draw.sType =
          VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier_from_present_to_draw.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      barrier_from_present_to_draw.dstAccessMask =
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      barrier_from_present_to_draw.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      barrier_from_present_to_draw.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      barrier_from_present_to_draw.srcQueueFamilyIndex =
          GetPresentQueue().FamilyIndex;
      barrier_from_present_to_draw.dstQueueFamilyIndex =
          GetGraphicsQueue().FamilyIndex;
      barrier_from_present_to_draw.image = swap_chain_images[i].Handle;
      barrier_from_present_to_draw.subresourceRange = image_subresource_range;
      vkCmdPipelineBarrier(graphics_command_buffers_[i],
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
                           nullptr, 0, nullptr, 1,
                           &barrier_from_present_to_draw);
    }

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass_;
    render_pass_begin_info.framebuffer = framebuffers_[i];
    render_pass_begin_info.renderArea = {{0, 0}, {800, 600}};
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(graphics_command_buffers_[i], &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(graphics_command_buffers_[i],
                      VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    vkCmdDraw(graphics_command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(graphics_command_buffers_[i]);

    if (GetGraphicsQueue().Handle != GetPresentQueue().Handle) {
      VkImageMemoryBarrier barrier_from_draw_to_present = {};
      barrier_from_draw_to_present.sType =
          VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier_from_draw_to_present.srcAccessMask =
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      barrier_from_draw_to_present.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
      barrier_from_draw_to_present.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      barrier_from_draw_to_present.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
      barrier_from_draw_to_present.srcQueueFamilyIndex =
          GetGraphicsQueue().FamilyIndex;
      barrier_from_draw_to_present.dstQueueFamilyIndex =
          GetPresentQueue().FamilyIndex;
      barrier_from_draw_to_present.image = swap_chain_images[i].Handle;
      barrier_from_draw_to_present.subresourceRange = image_subresource_range;

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

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &image_available_semaphore_;
  submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &graphics_command_buffers_[image_index];
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &rendering_finished_femaphore_;

  if (vkQueueSubmit(GetGraphicsQueue().Handle, 1, &submit_info,
                    VK_NULL_HANDLE) != VK_SUCCESS) {
    return false;
  }

  VkPresentInfoKHR present_info = {};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &rendering_finished_femaphore_;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swap_chain;
  present_info.pImageIndices = &image_index;

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

bool HelloTriangle::CreateVertexBuffer() {
  const std::vector<Vertex> vertices = {
      {{
          0.5f,
          -0.5f,
          0.0f,
      }},
      {{
          0.0f,
          0.5f,
          0.0f,
      }},
      {{
          -0.5f,
          -0.5f,
          0.0f,
      }},
  };
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = sizeof(vertices[0]) * vertices.size();
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateBuffer(GetDevice(), &buffer_info, nullptr, &vertex_buffer_) !=
      VK_SUCCESS) {
    return false;
  }
  if (vkCreateBuffer(GetDevice(), &buffer_info, nullptr,
                     &vertex_buffer_) != VK_SUCCESS) {
    std::cout << "Could not create a vertex buffer!" << std::endl;
    return false;
  }
}

 bool HelloTriangle::AllocateBufferMemory( VkBuffer buffer, VkDeviceMemory *memory ) {
    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements( GetDevice(), buffer, &buffer_memory_requirements );

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties( GetPhysicalDevice(), &memory_properties );

    for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i ) {
      if( (buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
        (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ) {

        VkMemoryAllocateInfo memory_allocate_info = {
          VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // VkStructureType                        sType
          nullptr,                                    // const void                            *pNext
          buffer_memory_requirements.size,            // VkDeviceSize                           allocationSize
          i                                           // uint32_t                               memoryTypeIndex
        };

        if( vkAllocateMemory( GetDevice(), &memory_allocate_info, nullptr, memory ) == VK_SUCCESS ) {
          return true;
        }
      }
    }
    return false;
  }
