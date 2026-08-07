#pragma once
#include "CharacterStats.h"
#include "Entity.h"
#include "Hitbox.h"
#include <PlayerStates.h>
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

  // State transition request (respects canExit guard)
  void requestState(PlayerState &state);

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
  void onClimb();

  // --- Movement helpers (called by States — Tell, Don't Ask) ---
  void idle();
  void moveRight();
  void moveLeft();
  void stopLeftRun();
  void stopRightRun();
  void jump();
  void crouch();
  void dash(float dashSpeed);
  void reduceMana(float cost);
  void increaseMana(float cost);
  void spawnFireball();

  // --- Swim & Climb helpers (avoids raw getRuntimeStatsMutable() in States) ---
  void swim(float dirX);   // Move horizontally at water speed
  void climbUp();          // Move up ladder
  void climbDown();        // Move down ladder
  void stopClimb();        // Stop on ladder

  // --- Combat (read-only queries for CombatSystem) ---
  bool hasActiveHitbox() const override;
  Hitbox getActiveHitbox() override;   // Non-const: avoids const_cast
  void takeDamage(int damage) override;

  // --- Polymorphic Hook overrides from Entity ---
  void onLand(float floorY) override;
  void onHitCeiling(float ceilY) override;
  void updateStateFromPhysics() override;
  void onEnterWater() override;
  void onOverlapLadder() override;
  void onHazard() override;
  void onDie() override;

  // --- Animation ---
  void playAnimation(const std::string &name);

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
};
