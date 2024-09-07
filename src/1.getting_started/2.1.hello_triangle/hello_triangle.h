#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "common/vulkan_common.h"

class HelloTriangle : public VulkanCommon {
public:
    bool    CreateRenderPass();
    bool    CreateFramebuffers();
    bool    CreatePipeline();
    bool    CreateSemaphores();
    bool    CreateCommandBuffers();
    bool    RecordCommandBuffers();
private:
    VkRenderPass render_pass_;
};
