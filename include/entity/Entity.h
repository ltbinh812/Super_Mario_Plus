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

public:
  Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
         const CharacterWorldStats &wS);
  virtual ~Entity() = default;
  virtual void update(float dt) = 0;
  virtual void render(float alpha) = 0;

  void addFloatingText(const std::string& text, Color color, Vector2 offset = {0, -20}, float lifetime = 1.0f);

  virtual bool getIsActive() const { return true; }
  virtual void deactivate() {}

  // Physics & Collision Template Method (Axis-Separated AABB Resolution)
  virtual void updatePhysicsWithMap(const TileMap& map, float dt);
  virtual void updatePhysicsSimple(float groundY, float dt);
  virtual Rectangle getHitbox() const;
  virtual void updateStateFromPhysics() {}

  // Combat interface — subclasses override as needed
  virtual bool hasActiveHitbox() const { return false; }
  virtual Hitbox getActiveHitbox() { return { {0,0,0,0}, 0, 0, nullptr }; }
  virtual void takeDamage(int damage, float knockbackDirX = 0.0f) {}

  // Polymorphic Hook Methods (Extension Points for subclasses)
  virtual void onHitWall(bool isRightWall) {}
  virtual void onLand(float floorY) {}
  virtual void onHitCeiling(float ceilY) {}
  virtual void onCollide(Entity& other) {}
  virtual void onEnterWater() {}
  virtual void onOverlapLadder() {}
  virtual void onHazard() {}
  virtual void onDie() {}

  virtual void dropThrough();

protected:
  void updateFloatingTexts(float dt);
  void renderFloatingTexts();

public:
  // Accessors — const versions for read-only
  const CharacterBaseStats &getBaseStats() const { return baseStats; }
  const CharacterRuntimeStats &getRuntimeStats() const { return runtimeStats; }
  const CharacterWorldStats &getWorldStats() const { return worldStats; }

  // Mutable accessors — only for subclasses that truly need direct mutation (e.g. State classes via Player helpers)
  CharacterBaseStats &getBaseStatsMutable() { return baseStats; }
  CharacterRuntimeStats &getRuntimeStatsMutable() { return runtimeStats; }
  CharacterWorldStats &getWorldStatsMutable() { return worldStats; }

  void setPosition(Vector2 pos) { worldStats.position = pos; }
  void setCommandQueue(CommandQueue* queue) { commandQueue = queue; }
};