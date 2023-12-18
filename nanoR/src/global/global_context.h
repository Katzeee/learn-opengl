#pragma once
#include "input/input_system.h"
#include "layer/ui_layer.h"
#include "nanorpch.h"

namespace nanoR {
class WindowBase;

template <Platform T>
class InputSystem;

class GlobalContext {
 public:
  static auto Instance() -> GlobalContext&;

  std::shared_ptr<WindowBase> window;
  // TODO: remove the strong dependency of Platform
  std::shared_ptr<InputSystem<Platform::Linux>> input_system;
  std::shared_ptr<UILayer> ui_layer;
};

}  // namespace nanoR