#pragma once
#include <pico_libs/ecs/settings.hpp>

#include "nanorpch.h"

namespace nanoR {

struct TransformComponent {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation;
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct MeshComponent {};

struct MaterialCompoenent {};

using Components = xac::mpl::type_list<TransformComponent>;
using Settings = xac::ecs::Settings<Components>;

}  // namespace nanoR