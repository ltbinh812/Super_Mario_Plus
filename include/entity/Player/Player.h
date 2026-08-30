#pragma once
#include "CharacterStats.h"
#include "Entity.h"
#include "Hitbox.h"
#include "BuffManager.h"
#include "PlayerStates.h"
#include "ISkill.h"
#include <map>
#include <memory>
#include <string>
#include <unordered_map>


class ISkill;

class Player : public Entity {
  friend class PlayerState; // Needed for changePlayerState() delegation

public:
  Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS,
         CharacterWorldStats &wS,
         std::unordered_map<std::string, Animation> animations);
  void update(float dt) override;
  void render(float alpha) override;

  void setPartyInventory(std::shared_ptr<PartyInventory> inv) { partyInventory_ = inv; }
  std::shared_ptr<PartyInventory> getPartyInventory() const { return partyInventory_; }
  BuffManager& getBuffManager() { return buffManager_; }
  const BuffManager& getBuffManager() const { return buffManager_; }
  CommandQueue* getCommandQueue() const { return commandQueue; }

  // State transition request (respects canExit guard)
  void requestState(PlayerState &state);
  // Force a state transition (ignores canExit guard, e.g. for respawning)
  void forceState(PlayerState &state);

  // Life cycle encapsulation
  bool isDead() const;
  bool isOutOfBounds(float limitY) const;
  void respawn(Vector2 startPos);

  // Skill system
  void useSkill(const std::string &skillname);
  void addSkill(const std::string &name, std::unique_ptr<ISkill> skill);
  ISkill* findSkill(const std::string& skillName);
  bool hasEnoughMana(float cost) const;

  // --- Input action dispatchers (called by Command Pattern) ---
  void onMoveRight();
  void onMoveLeft();
  void onJump();
  void onStopLeft();
  void onStopRight();
  void onCrouch();
  void onStopCrouch();
  void onAttack();
  void dropThrough();
  void interactWithOverlapping(); // Swap/pickup when overlapping item; throw when not
  void useStoredItem();           // Use or throw stored item via Strategy Pattern
  void setOverlappingItem(class BaseItem* item) { overlappingItem_ = item; }
  void onClimb();

  // --- Movement helpers (called by States — Tell, Don't Ask) ---
  void idle();
  void moveRight();
  void moveLeft();
  void stopLeftRun();
  void stopRightRun();
  void jump();
  void crouch();
  void standUp();
  void speedUpX(float speedX);
  void speedUpY(float speedY);
  void reduceMana(float cost);
  void increaseMana(float cost);
  void spawnFireball();
  void spawnExplosion();
  void spawnSpecialBall();

  // --- Swim & Climb helpers (avoids raw getRuntimeStatsMutable() in States) ---
  void swim(float dirX);   // Move horizontally at water speed
  void swimY(float dirY);
  void climbUp();          // Move up ladder
  void climbDown();        // Move down ladder
  void stopClimb();        // Stop on ladder

  // --- Combat (read-only queries for CombatSystem) ---
  bool hasActiveHitbox() const override;
  Hitbox getActiveHitbox() override;   // Non-const: avoids const_cast
  void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;
  void setPvPMode(bool pvp) { isPvPMode_ = pvp; }
  bool getPvPMode() const { return isPvPMode_; }

  // --- Polymorphic Hook overrides from Entity ---
  void onLand(float floorY) override;
  void onHitCeiling(float ceilY) override;
  void updateStateFromPhysics() override;
  void onEnterWater() override;
  void onExitLiquid();
  void onOverlapLadder() override;
  void onHazard() override;
  void onDie() override;
  void onCutsceneStart(const std::string& triggerId) override;

  // --- Animation ---
  void playAnimation(const std::string &name, bool loop = true);

  // --- State instances (public so States can reference siblings) ---
  PlayerIdleState idleState;
  PlayerRunState runState;
  PlayerJumpState jumpState;
  PlayerFallState fallState;
  PlayerCrouchState crouchState;
  PlayerHurtState hurtState;
  PlayerDieState dieState;
  PlayerSkillState skillState;
  PlayerSwimState swimState;
  PlayerClimbState climbState;

private:
  void changeState(PlayerState &state);

  PlayerState *currentState;
  std::unordered_map<std::string, Animation> animationList;
  std::unordered_map<std::string, std::unique_ptr<ISkill>> skillList;
  class BaseItem* overlappingItem_ = nullptr;
  std::shared_ptr<PartyInventory> partyInventory_;
  BuffManager buffManager_;
  bool isPvPMode_ = false;
};
