#include "vulkan_common.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

VulkanCommon::VulkanCommon() {}
VulkanCommon::~VulkanCommon() {}

bool VulkanCommon::CheckExtensionAvailability(
    const char *extension_name,
    const std::vector<VkExtensionProperties> &available_extensions) {
  for (std::size_t i = 0; i < available_extensions.size(); ++i) {
    if (strcmp(available_extensions[i].extensionName, extension_name) == 0) {
      return true;
    }
  }
  return false;
}

bool VulkanCommon::CreateInstance() {
  uint32_t extensions_count = 0;
  if ((vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count,
                                              nullptr) != VK_SUCCESS) ||
      (extensions_count == 0)) {
    std::cout << "Error occurred during instance extensions enumeration!"
              << std::endl;
    return false;
  }

  std::vector<VkExtensionProperties> available_extensions(extensions_count);
  if (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count,
                                             available_extensions.data()) !=
      VK_SUCCESS) {
    std::cout << "Error occurred during instance extensions enumeration!"
              << std::endl;
    return false;
  }

  std::vector<const char *> extensions = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
  };

  for (std::size_t i = 0; i < extensions.size(); ++i) {
    if (!CheckExtensionAvailability(extensions[i], available_extensions)) {
      std::cout << "Could not find instance extension named \"" << extensions[i]
                << "\"!" << std::endl;
      return false;
    }
  }

  VkApplicationInfo application_info = {
      VK_STRUCTURE_TYPE_APPLICATION_INFO,  // VkStructureType            sType
      nullptr,                             // const void                *pNext
      "API without Secrets: Introduction to Vulkan",  // const char
                                                      // *pApplicationName
      VK_MAKE_VERSION(1, 0, 0),    // uint32_t applicationVersion
      "Vulkan Tutorial by Intel",  // const char                *pEngineName
      VK_MAKE_VERSION(1, 0, 0),    // uint32_t                   engineVersion
      VK_MAKE_VERSION(1, 0, 0)     // uint32_t                   apiVersion
  };

  VkInstanceCreateInfo instance_create_info = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // VkStructureType sType
      nullptr,            // const void                *pNext
      0,                  // VkInstanceCreateFlags      flags
      &application_info,  // const VkApplicationInfo   *pApplicationInfo
      0,                  // uint32_t                   enabledLayerCount
      nullptr,            // const char * const        *ppEnabledLayerNames
      static_cast<uint32_t>(
          extensions.size()),  // uint32_t enabledExtensionCount
      extensions.data()  // const char * const        *ppEnabledExtensionNames
  };

  if (vkCreateInstance(&instance_create_info, nullptr, &Vulkan.Instance) !=
      VK_SUCCESS) {
    std::cout << "Could not create Vulkan instance!" << std::endl;
    return false;
  }
  return true;
}

bool VulkanCommon::CheckPhysicalDeviceProperties(
    VkPhysicalDevice physical_device,
    uint32_t &selected_graphics_queue_family_index,
    uint32_t &selected_present_queue_family_index) {
  uint32_t extensions_count = 0;
  if ((vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                            &extensions_count,
                                            nullptr) != VK_SUCCESS) ||
      (extensions_count == 0)) {
    std::cout << "Error occurred during physical device " << physical_device
              << " extensions enumeration!" << std::endl;
    return false;
  }

  std::vector<VkExtensionProperties> available_extensions(extensions_count);
  if (vkEnumerateDeviceExtensionProperties(
          physical_device, nullptr, &extensions_count,
          available_extensions.data()) != VK_SUCCESS) {
    std::cout << "Error occurred during physical device " << physical_device
              << " extensions enumeration!" << std::endl;
    return false;
  }

  std::vector<const char *> device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  for (std::size_t i = 0; i < device_extensions.size(); ++i) {
    if (!CheckExtensionAvailability(device_extensions[i],
                                    available_extensions)) {
      std::cout << "Physical device " << physical_device
                << " doesn't support extension named \"" << device_extensions[i]
                << "\"!" << std::endl;
      return false;
    }
  }

  VkPhysicalDeviceProperties device_properties;
  VkPhysicalDeviceFeatures device_features;

  vkGetPhysicalDeviceProperties(physical_device, &device_properties);
  vkGetPhysicalDeviceFeatures(physical_device, &device_features);

  uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);

  if ((major_version < 1) ||
      (device_properties.limits.maxImageDimension2D < 4096)) {
    std::cout << "Physical device " << physical_device
              << " doesn't support required parameters!" << std::endl;
    return false;
  }

  uint32_t queue_families_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_families_count, nullptr);
  if (queue_families_count == 0) {
    std::cout << "Physical device " << physical_device
              << " doesn't have any queue families!" << std::endl;
    return false;
  }

  std::vector<VkQueueFamilyProperties> queue_family_properties(
      queue_families_count);
  std::vector<VkBool32> queue_present_support(queue_families_count);

  vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device, &queue_families_count, queue_family_properties.data());

  uint32_t graphics_queue_family_index = UINT32_MAX;
  uint32_t present_queue_family_index = UINT32_MAX;

  for (uint32_t i = 0; i < queue_families_count; ++i) {
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i,
                                         Vulkan.PresentationSurface,
                                         &queue_present_support[i]);

    if ((queue_family_properties[i].queueCount > 0) &&
        (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      // Select first queue that supports graphics
      if (graphics_queue_family_index == UINT32_MAX) {
        graphics_queue_family_index = i;
      }

      // If there is queue that supports both graphics and present - prefer it
      if (queue_present_support[i]) {
        selected_graphics_queue_family_index = i;
        selected_present_queue_family_index = i;
        return true;
      }
    }
  }

  // We don't have queue that supports both graphics and present so we have to
  // use separate queues
  for (uint32_t i = 0; i < queue_families_count; ++i) {
    if (queue_present_support[i]) {
      present_queue_family_index = i;
      break;
    }
  }

  // If this device doesn't support queues with graphics and present
  // capabilities don't use it
  if ((graphics_queue_family_index == UINT32_MAX) ||
      (present_queue_family_index == UINT32_MAX)) {
    std::cout << "Could not find queue families with required properties on "
                 "physical device "
              << physical_device << "!" << std::endl;
    return false;
  }

  selected_graphics_queue_family_index = graphics_queue_family_index;
  selected_present_queue_family_index = present_queue_family_index;
  return true;
}

bool VulkanCommon::CreateDevice() {
  uint32_t num_devices = 0;
  if ((vkEnumeratePhysicalDevices(Vulkan.Instance, &num_devices, nullptr) !=
       VK_SUCCESS) ||
      (num_devices == 0)) {
    std::cout << "Error occurred during physical devices enumeration!"
              << std::endl;
    return false;
  }

  std::vector<VkPhysicalDevice> physical_devices(num_devices);
  if (vkEnumeratePhysicalDevices(Vulkan.Instance, &num_devices,
                                 physical_devices.data()) != VK_SUCCESS) {
    std::cout << "Error occurred during physical devices enumeration!"
              << std::endl;
    return false;
  }

  uint32_t selected_graphics_queue_family_index = UINT32_MAX;
  uint32_t selected_present_queue_family_index = UINT32_MAX;

  for (uint32_t i = 0; i < num_devices; ++i) {
    if (CheckPhysicalDeviceProperties(physical_devices[i],
                                      selected_graphics_queue_family_index,
                                      selected_present_queue_family_index)) {
      Vulkan.PhysicalDevice = physical_devices[i];
      break;
    }
  }
  if (Vulkan.PhysicalDevice == VK_NULL_HANDLE) {
    std::cout
        << "Could not select physical device based on the chosen properties!"
        << std::endl;
    return false;
  }

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::vector<float> queue_priorities = {1.0f};

  queue_create_infos.push_back({
      VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void                  *pNext
      0,        // VkDeviceQueueCreateFlags     flags
      selected_graphics_queue_family_index,  // uint32_t queueFamilyIndex
      static_cast<uint32_t>(queue_priorities.size()),  // uint32_t queueCount
      queue_priorities.data()  // const float *pQueuePriorities
  });

  if (selected_graphics_queue_family_index !=
      selected_present_queue_family_index) {
    queue_create_infos.push_back({
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // VkStructureType sType
        nullptr,  // const void                  *pNext
        0,        // VkDeviceQueueCreateFlags     flags
        selected_present_queue_family_index,  // uint32_t queueFamilyIndex
        static_cast<uint32_t>(queue_priorities.size()),  // uint32_t queueCount
        queue_priorities.data()  // const float *pQueuePriorities
    });
  }

  std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDeviceCreateInfo device_create_info = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // VkStructureType sType
      nullptr,  // const void                        *pNext
      0,        // VkDeviceCreateFlags                flags
      static_cast<uint32_t>(
          queue_create_infos.size()),  // uint32_t queueCreateInfoCount
      queue_create_infos
          .data(),  // const VkDeviceQueueCreateInfo     *pQueueCreateInfos
      0,            // uint32_t                           enabledLayerCount
      nullptr,      // const char * const                *ppEnabledLayerNames
      static_cast<uint32_t>(
          extensions.size()),  // uint32_t enabledExtensionCount
      extensions.data(),       // const char * const *ppEnabledExtensionNames
      nullptr  // const VkPhysicalDeviceFeatures    *pEnabledFeatures
  };

  if (vkCreateDevice(Vulkan.PhysicalDevice, &device_create_info, nullptr,
                     &Vulkan.Device) != VK_SUCCESS) {
    std::cout << "Could not create Vulkan device!" << std::endl;
    return false;
  }

  Vulkan.GraphicsQueue.FamilyIndex = selected_graphics_queue_family_index;
  Vulkan.PresentQueue.FamilyIndex = selected_present_queue_family_index;
  return true;
}