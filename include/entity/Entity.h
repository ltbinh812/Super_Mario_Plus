#pragma once
#include "CharacterStats.h"
#include "IEntityState.h"
#include "Hitbox.h"
#include "raylib.h"
#include "CommandQueue.h"
#include "Effects.h"
#include <vector>
#include <memory>


class TileMap;

class Entity {
protected:
  CharacterBaseStats baseStats;
  CharacterRuntimeStats runtimeStats;
  CharacterWorldStats worldStats;
  CommandQueue* commandQueue = nullptr;
  std::string iid_; // Unique identifier from LDtk map
  std::vector<std::unique_ptr<IEffect>> activeEffects;

  // Internal physics helpers
  void handleTriggers(const TileMap& map, float dt);
  void applyGravity(float dt);
  void resolveCollisionX(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);
  void resolveCollisionY(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);

public:
  Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
         const CharacterWorldStats &wS);
  virtual ~Entity() = default;
  virtual void update(float dt) = 0;
  virtual void render(float alpha) = 0;

  virtual bool getIsActive() const { return true; }
  virtual void deactivate() {}

  // Physics & Collision Template Method (Axis-Separated AABB Resolution)
  virtual void updatePhysicsWithMap(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);
  virtual void updatePhysicsSimple(float groundY, float dt);
  virtual Rectangle getHitbox() const;
  virtual void updateStateFromPhysics() {}

  // Combat interface — subclasses override as needed
  virtual bool hasActiveHitbox() const { return false; }
  virtual Hitbox getActiveHitbox() { return { {0,0,0,0}, 0, 0, nullptr }; }
  virtual void takeDamage(int damage, bool forceInterrupt = true) {}

  // Polymorphic Hook Methods (Extension Points for subclasses)
  virtual void onHitWall(bool isRightWall) {}
  virtual void onLand(float floorY) {}
  virtual void onHitCeiling(float ceilY) {}
  virtual void onCollide(Entity& other) {}
  virtual void onEnterWater() {}
  virtual void onExitLiquid() {}
  virtual void onOverlapLadder() {}
  virtual void onHazard() {}
  virtual void onDie() {}

  virtual void dropThrough();

  void addEffect(std::unique_ptr<IEffect> effect);
  bool hasEffect(const std::string& name) const;
  void updateEffects(float dt);
  void clearEffects();

  // Accessors — const versions for read-only
  const CharacterBaseStats &getBaseStats() const { return baseStats; }
  const CharacterRuntimeStats &getRuntimeStats() const { return runtimeStats; }
  const CharacterWorldStats &getWorldStats() const { return worldStats; }

  // Mutable accessors — only for subclasses that truly need direct mutation (e.g. State classes via Player helpers)
  CharacterBaseStats &getBaseStatsMutable() { return baseStats; }
  CharacterRuntimeStats &getRuntimeStatsMutable() { return runtimeStats; }
  CharacterWorldStats &getWorldStatsMutable() { return worldStats; }

  void setPosition(Vector2 pos) { 
      worldStats.position = pos; 
  }
  void setStartPosition(Vector2 pos) { 
      worldStats.startPosition = pos; 
  }
  void setCommandQueue(CommandQueue* cq) { commandQueue = cq; }

  const std::string& getIid() const { return iid_; }
  void setIid(const std::string& iid) { iid_ = iid; }
};