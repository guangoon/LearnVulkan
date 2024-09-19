#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common/tools.h"
#include "common/vulkan_common.h"

class HelloTriangle : public VulkanCommon {
 public:
  HelloTriangle();
  ~HelloTriangle();
  bool CreateRenderPass();
  bool CreateFramebuffers();
  bool CreatePipeline();
  bool CreateSemaphores();
  bool CreateCommandBuffers();
  bool RecordCommandBuffers();
  bool Draw() override;

 private:
  void ChildClear() override;
  bool ChildOnWindowSizeChanged() override;
  Tools::AutoDeleter<VkShaderModule, PFN_vkDestroyShaderModule>
  CreateShaderModule(const char* filename);
  Tools::AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>
  CreatePipelineLayout();
  bool CreateCommandPool(uint32_t queue_family_index, VkCommandPool* pool);
  bool AllocateCommandBuffers(VkCommandPool pool, uint32_t count,
                              VkCommandBuffer* command_buffers);
  VkRenderPass render_pass_;
  std::vector<VkFramebuffer> framebuffers_;
  VkPipeline graphics_pipeline_;
  VkSemaphore image_available_semaphore_;
  VkSemaphore rendering_finished_femaphore_;
  VkCommandPool graphics_command_pool_;
  std::vector<VkCommandBuffer> graphics_command_buffers_;
};
