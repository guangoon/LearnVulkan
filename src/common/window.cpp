#include "window.h"

#include <iostream>

Window::Window() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}
Window::~Window() {
  if (window_) {
    glfwDestroyWindow(window_);
  }
  glfwTerminate();
}

bool Window::Create(const char *title, int width, int height) {
  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window_ == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return false;
  }
  return true;
}

GLFWwindow *Window::GetWindow() { return window_; }

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Window::ProcessInput() {
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window_, true);
}

bool Window::RenderingLoop(VulkanCommon &vulkan_common) {
  while (!glfwWindowShouldClose(window_)) {
    // input
    // -----
    ProcessInput();
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    
    vulkan_common.Draw();
    glfwPollEvents();
  }
  return true;
}