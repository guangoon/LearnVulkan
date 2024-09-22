
#ifndef VULKAN_COMMON_H_
#define VULKAN_COMMON_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>

// ************************************************************ //
// QueueParameters                                              //
//                                                              //
// Vulkan Queue's parameters container class                    //
// ************************************************************ //
struct QueueParameters {
  VkQueue Handle;
  uint32_t FamilyIndex;

  QueueParameters() : Handle(VK_NULL_HANDLE), FamilyIndex(0) {}
};

// ************************************************************ //
// ImageParameters                                              //
//                                                              //
// Vulkan Image's parameters container class                    //
// ************************************************************ //
struct ImageParameters {
  VkImage Handle;
  VkImageView View;
  VkSampler Sampler;
  VkDeviceMemory Memory;

  ImageParameters()
      : Handle(VK_NULL_HANDLE),
        View(VK_NULL_HANDLE),
        Sampler(VK_NULL_HANDLE),
        Memory(VK_NULL_HANDLE) {}
};

// ************************************************************ //
// SwapChainParameters                                          //
//                                                              //
// Vulkan SwapChain's parameters container class                //
// ************************************************************ //
struct SwapChainParameters {
  VkSwapchainKHR Handle;
  VkFormat Format;
  std::vector<ImageParameters> Images;
  VkExtent2D Extent;

  SwapChainParameters()
      : Handle(VK_NULL_HANDLE),
        Format(VK_FORMAT_UNDEFINED),
        Images(),
        Extent() {}
};

// ************************************************************ //
// VulkanCommonParameters                                       //
//                                                              //
// General Vulkan parameters' container class                   //
// ************************************************************ //
struct VulkanCommonParameters {
  VkInstance Instance;
  VkPhysicalDevice PhysicalDevice;
  VkDevice Device;
  QueueParameters GraphicsQueue;
  QueueParameters PresentQueue;
  VkSurfaceKHR PresentationSurface;
  SwapChainParameters SwapChain;

  VulkanCommonParameters()
      : Instance(VK_NULL_HANDLE),
        PhysicalDevice(VK_NULL_HANDLE),
        Device(VK_NULL_HANDLE),
        GraphicsQueue(),
        PresentQueue(),
        PresentationSurface(VK_NULL_HANDLE),
        SwapChain() {}
};

class VulkanCommon {
 public:
  VulkanCommon();
  virtual ~VulkanCommon();
  VkDevice GetDevice() const;
  const SwapChainParameters &GetSwapChain() const;
  bool PrepareVulkan(GLFWwindow *window);
  const QueueParameters GetGraphicsQueue() const;
  const QueueParameters GetPresentQueue() const;
  VkPhysicalDevice GetPhysicalDevice() const;
  bool OnWindowSizeChanged();
  virtual bool Draw() = 0;
  virtual bool ReadyToDraw() const final { return can_render_; }

 private:
  bool CheckExtensionAvailability(
      const char *extension_name,
      const std::vector<VkExtensionProperties> &available_extensions);
  bool CheckPhysicalDeviceProperties(
      VkPhysicalDevice physical_device,
      uint32_t &selected_graphics_queue_family_index,
      uint32_t &selected_present_queue_family_index);
  virtual bool ChildOnWindowSizeChanged() = 0;
  virtual void ChildClear() = 0;
  bool CreateInstance();
  bool CreateDevice();
  bool CreatePresentationSurface(GLFWwindow *window);
  bool CreateSwapChain();
  bool CreateSwapChainImageViews();
  bool GetDeviceQueue();
  
  std::vector<const char *> GetRequiredExtensions();

  uint32_t GetSwapChainNumImages(
      VkSurfaceCapabilitiesKHR &surface_capabilities);
  VkSurfaceFormatKHR GetSwapChainFormat(
      std::vector<VkSurfaceFormatKHR> &surface_formats);
  VkExtent2D GetSwapChainExtent(VkSurfaceCapabilitiesKHR &surface_capabilities);
  VkImageUsageFlags GetSwapChainUsageFlags(
      VkSurfaceCapabilitiesKHR &surface_capabilities);
  VkSurfaceTransformFlagBitsKHR GetSwapChainTransform(
      VkSurfaceCapabilitiesKHR &surface_capabilities);
  VkPresentModeKHR GetSwapChainPresentMode(
      std::vector<VkPresentModeKHR> &present_modes);
  bool can_render_;
  VulkanCommonParameters vulkan_;
};

#endif