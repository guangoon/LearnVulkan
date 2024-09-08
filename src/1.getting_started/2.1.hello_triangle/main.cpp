////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

#include "hello_triangle.h"
#include "window.h"
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main(int argc, char **argv) {
  Window window;
  HelloTriangle helloTriangle;
  // Window creation
  if (!window.Create("Hello, triangle", WIDTH, HEIGHT)) {
    return -1;
  }

  // Vulkan preparations and initialization
  if (!helloTriangle.PrepareVulkan(window.GetWindow())) {
    return -1;
  }

  std::cout << "CreateRenderPass" << std::endl;
  if (!helloTriangle.CreateRenderPass()) {
    return -1;
  }
   std::cout << "CreateFramebuffers" << std::endl;
  if (!helloTriangle.CreateFramebuffers()) {
    return -1;
  }
  std::cout << "CreatePipeline" << std::endl;
  if (!helloTriangle.CreatePipeline()) {
    return -1;
  }
  std::cout << "CreateSemaphores" << std::endl;
  if (!helloTriangle.CreateSemaphores()) {
    return -1;
  }
  std::cout << "CreateCommandBuffers" << std::endl;
  if (!helloTriangle.CreateCommandBuffers()) {
    return -1;
  }
  std::cout << "RecordCommandBuffers" << std::endl;
  if (!helloTriangle.RecordCommandBuffers()) {
    return -1;
  }

  // Rendering loop
  if (!window.RenderingLoop()) {
    return -1;
  }
  return 0;
}
