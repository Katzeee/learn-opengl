#pragma once
#include <memory>

namespace xac {

enum class ControlCommand : uint32_t;
class Camera;
class InputSystem;

class GlobalContext {
 public:
  GlobalContext() = default;
  ~GlobalContext() = default;

  void Init();

  std::unique_ptr<InputSystem> input_system_;
  std::unique_ptr<Camera> camera_;

  uint32_t control_commad_ = 0;
  int window_width_ = 1300;
  int window_height_ = 600;
  int imgui_width_ = 340;

 private:
};

auto InCommand(xac::ControlCommand command) -> bool;

}  // end namespace xac

extern xac::GlobalContext global_context;