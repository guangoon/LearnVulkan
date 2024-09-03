
#ifndef VULKAN_COMMON_H_
#define VULKAN_COMMON_H_

#define GLFW_INCLUDE_VULKAN

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

 private:
  bool CheckExtensionAvailability(
      const char *extension_name,
      const std::vector<VkExtensionProperties> &available_extensions);
  bool CreateInstance();
  bool CheckPhysicalDeviceProperties(
      VkPhysicalDevice physical_device,
      uint32_t &selected_graphics_queue_family_index,
      uint32_t &selected_present_queue_family_index);
  bool CreateDevice();

  VulkanCommonParameters Vulkan;
};

#endif