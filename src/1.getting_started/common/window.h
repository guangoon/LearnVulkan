#ifndef WINDOW_H_
#define WINDOW_H_

#include <GLFW/glfw3.h>

class Window {
 public:
  Window();
  ~Window();

  bool Create(const char *title, int width, int height);
  bool RenderingLoop() const;

 private:
  GLFWwindow *window_ = nullptr;
};

#endif