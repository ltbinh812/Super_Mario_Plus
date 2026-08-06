#pragma once
#include "CharacterStats.h"
#include "IEntityState.h"
#include "Hitbox.h"
#include "raylib.h"
#include "CommandQueue.h"


class TileMap;

class Entity {
protected:
  CharacterBaseStats baseStats;
  CharacterRuntimeStats runtimeStats;
  CharacterWorldStats worldStats;
  CommandQueue* commandQueue = nullptr;

  // Subclasses can use the commandQueue directly to spawn entities

public:
  Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
         const CharacterWorldStats &wS);
  virtual ~Entity() = default;
  virtual void update(float dt) = 0;
  virtual void render(float alpha) = 0;

  virtual bool getIsActive() const { return true; }
  virtual void deactivate() {}

  // Physics & Collision Template Method (Axis-Separated AABB Resolution)
  virtual void updatePhysicsWithMap(const TileMap& map, float dt);
  virtual void updatePhysicsSimple(float groundY, float dt); // Dùng cho cảnh không có TileMap (ví dụ IntroState)
  virtual Rectangle getHitbox() const;
  virtual void updateStateFromPhysics() {}

  // Combat interface — subclasses override as needed
  virtual bool hasActiveHitbox() const { return false; }
  virtual Hitbox getActiveHitbox() const { return { {0,0,0,0}, 0, 0, nullptr }; }
  virtual void takeDamage(int damage) {}

  // Polymorphic Hook Methods (Extension Points for subclasses)
  virtual void onHitWall(bool isRightWall) {}
  virtual void onLand(float floorY) {}
  virtual void onHitCeiling(float ceilY) {}
  virtual void onCollide(Entity& other) {} // Extension point for future Entity vs Entity collision

  const CharacterBaseStats &getBaseStats() const { return baseStats; }
  const CharacterRuntimeStats &getRuntimeStats() const { return runtimeStats; }
  const CharacterWorldStats &getWorldStats() const { return worldStats; }
  
  void setCommandQueue(CommandQueue* queue) { commandQueue = queue; }
};