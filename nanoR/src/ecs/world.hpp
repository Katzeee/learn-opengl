#pragma once
#include <assert.h>

#include <algorithm>
#include <bitset>
#include <memory>
#include <vector>

#include "component.hpp"
#include "entity.hpp"
#include "mpu.hpp"
#include "settings.hpp"
namespace xac::ecs {
template <typename TSettings>
class Entity;

constexpr static uint32_t kInitSize = 200;

template <typename TSettings>
class World {
 public:
  using ComponentList = typename TSettings::ComponentList;
  template <typename... Args>
  using TupleOfVectors = std::tuple<std::vector<Args>...>;
  using Entity = Entity<TSettings>;
  using EntityId = typename Entity::Id;

 public:
  World();
  auto create() -> EntityId;
  template <typename T, typename... Args>
  auto assign(EntityId id, Args... args) -> std::shared_ptr<ComponentHandle>;
  template <typename F>
  auto each(F &&f);

 private:
  auto prepare_entity_create() -> void;
  auto invalidate(EntityId id) -> void;

 private:
  mpu::rename<TupleOfVectors, ComponentList> components_pool_;
  std::vector<Entity> entities_;
  std::vector<uint32_t> entity_version_;
  static uint32_t entity_count_;
};

template <typename TSettings>
inline uint32_t World<TSettings>::entity_count_ = 0;

template <typename TSettings>
World<TSettings>::World() {
  entities_.resize(kInitSize);
  entity_version_.resize(kInitSize);
}

template <typename TSettings>
auto World<TSettings>::create() -> EntityId {
  prepare_entity_create();
  auto id = EntityId();
  id.id = entity_count_;
  id.version = ++entity_version_[entity_count_];
  entities_[entity_count_] = std::move(Entity(id, this));
  entity_count_++;
  return id;
}

template <typename TSettings>
template <typename T, typename... Args>
auto World<TSettings>::assign(EntityId id, Args... args) -> std::shared_ptr<ComponentHandle> {
  static_assert(TSettings::template has_component<T>(), "type is not in component list");
  invalidate(id);
  auto &entity = entities_[id.id];
  auto &version = entity_version_[id.id];
  version++;
  entity.id_.version = version;
  entity.components_mask_.set(mpu::type_id::value<T>);
  return nullptr;
}

template <typename TSettings>
template <typename F>
auto World<TSettings>::each(F &&f) {
  for (auto i = 0; i < entity_count_; i++) {
    std::invoke(f, entities_[i]);
  }
}

template <typename TSettings>
auto World<TSettings>::invalidate(EntityId id) -> void {
  auto eid = id.id;
  auto eversion = id.version;
  assert(entities_[eid].id_.version == eversion && "This id is out of date");
  assert(entity_version_[eid] == eversion && "This id is out of date");
}

template <typename TSettings>
auto World<TSettings>::prepare_entity_create() -> void {
  assert(entities_.size() == entity_version_.size());
  if (entity_count_ >= entities_.size()) {
    entities_.resize(entity_count_ * 2);  // HINT: must be resize, because you will use index
    entity_version_.resize(entity_count_ * 2);
  }
}

}  // namespace xac::ecs