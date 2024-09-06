#include "window.h"

Window::Window() {}
Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Window::Create(const char *title, int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  return window_ != nullptr;
}

bool Window::RenderingLoop() const {
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
  }
  return true;
}