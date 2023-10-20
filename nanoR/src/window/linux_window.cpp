#include "linux_window.h"
#include "window.h"

namespace nanoR {
LinuxWindow::LinuxWindow(WindowProp window_prop) {
  if (!window_) {
    Init(window_prop);
  }
}
auto LinuxWindow::Init(WindowProp window_prop) -> void {
  if (!glfwInit()) {
    LOG_FATAL("GLFW initialize failed!\n");
    glfwTerminate();
    throw std::runtime_error("GLFW initialize failed!");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHintString(GLFW_X11_CLASS_NAME, "opengl test");
  glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "opengl test");
  window_ = glfwCreateWindow(window_prop.width, window_prop.height, window_prop.title, nullptr, nullptr);
  if (!window_) {
    LOG_FATAL("Window initialize failed!\n");
    throw std::runtime_error("Window initialize failed!");
  }
  LOG_INFO("Window initialize with width {} height {} title \"{}\".\n", window_prop.width, window_prop.height,
           window_prop.title);
  glfwMakeContextCurrent(window_);
}

auto LinuxWindow::Tick() -> void {
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

auto LinuxWindow::Shutdown() -> void {
  glfwDestroyWindow(window_);
  window_ = nullptr;
}

LinuxWindow::~LinuxWindow() { Shutdown(); }
}  // namespace nanoR
