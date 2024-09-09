#ifndef WINDOW_H_
#define WINDOW_H_
#include <GLFW/glfw3.h>

#include "common/vulkan_common.h"

class Window {
 public:
  Window();
  ~Window();

  bool Create(const char *title, int width, int height);
  GLFWwindow *GetWindow();
  bool RenderingLoop(VulkanCommon &vulkan_common);

 private:
  GLFWwindow *window_ = nullptr;
  void ProcessInput();
};

#endif