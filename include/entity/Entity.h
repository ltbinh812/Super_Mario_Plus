#pragma once
#include "CharacterStats.h"
#include "IEntityState.h"
#include "Hitbox.h"
#include "EntityFaction.h"
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
  
protected:
  std::string currentSoundKey = "";
  
  std::vector<std::unique_ptr<IEffect>> activeEffects;
  std::vector<class Player*> targetPlayers; // References to players in the levelrs
  EntityFaction faction = EntityFaction::None;

  // Internal physics helpers
  void handleTriggers(const TileMap& map, float dt);
  void checkEdgeAndWater(const TileMap& map);
  void applyGravity(float dt);
  void resolveCollisionX(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);
  void resolveCollisionY(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);

public:
  Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
         const CharacterWorldStats &wS);
  virtual ~Entity();
  virtual void update(float dt) = 0;
  virtual void render(float alpha) = 0;
  virtual void decideAction() {}
  virtual void process() {}

  void setCurrentSoundKey(const std::string& key) { currentSoundKey = key; }

  void addFloatingText(const std::string& text, Color color, Vector2 offset = {0, -20}, float lifetime = 1.0f);

  virtual bool getIsActive() const { return true; }
  virtual void deactivate() {}
  
  virtual void setTargetPlayers(const std::vector<class Player*>& players) {}

  // Physics & Collision Template Method (Axis-Separated AABB Resolution)
  virtual void updatePhysicsWithMap(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt);
  virtual void updatePhysicsSimple(float groundY, float dt);
  virtual Rectangle getHitbox() const;
  virtual void updateStateFromPhysics() {}

  // Combat interface — subclasses override as needed
  virtual bool hasActiveHitbox() const { return false; }
  virtual Hitbox getActiveHitbox() { return { {0,0,0,0}, 0, 0, nullptr }; }
  virtual void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) {}

  // Đòn của thực thể này vừa CHẠM được mục tiêu. CombatSystem gọi ngược về chủ
  // hitbox ngay sau khi đã trừ máu — trước đây không có đường phản hồi nào nên
  // kẻ tấn công không hề biết mình đánh trúng hay đánh hụt.
  // Player dùng nó để kích hoạt hit-stop (xem Player::onDealtDamage).
  virtual void onDealtDamage(Entity* target, int amount) {}

  // Polymorphic Hook Methods (Extension Points for subclasses)
  virtual void onHitWall(bool isRightWall, bool isCliff = false) {}
  virtual void onLand(float floorY) {}
  virtual void onHitCeiling(float ceilY) {}
  virtual void onCollide(Entity& other) {}
  virtual void onEnterWater() {}
  virtual void onExitLiquid() {}
  virtual void onOverlapLadder() {}
  virtual void onHazard() {}
  virtual void onDie() {}

  virtual void dropThrough();

  virtual void onCutsceneStart(const std::string& triggerId) {}
  virtual void onCutsceneEnd(const std::string& triggerId) {}

  void addEffect(std::unique_ptr<IEffect> effect);
  bool hasEffect(const std::string& name) const;
  void updateEffects(float dt);
  void clearEffects();

protected:
  void updateFloatingTexts(float dt);
  void renderFloatingTexts();

public:
  // Accessors — const versions for read-only
  const CharacterBaseStats &getBaseStats() const { return baseStats; }
  const CharacterRuntimeStats &getRuntimeStats() const { return runtimeStats; }
  const CharacterWorldStats &getWorldStats() const { return worldStats; }
  Vector2 getPosition() const { return worldStats.position; }

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
  CommandQueue* getCommandQueue() const { return commandQueue; }

  const std::string& getIid() const { return iid_; }
  void setIid(const std::string& iid) { iid_ = iid; }
  
  EntityFaction getFaction() const { return faction; }
  void setFaction(EntityFaction f) { faction = f; }

  virtual bool isPvPEnabled() const { return false; }
};