#pragma once
#include "CharacterStats.h"
#include "Entity.h"
#include <PlayerStates.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>


class ISkill;

class Player : public Entity {
  friend class PlayerState;

public:
  Player(CharacterBaseStats &bS, CharacterRuntimeStats &rS,
         CharacterWorldStats &wS,
         std::unordered_map<std::string, Animation> animations);
  void update(float dt) override;
  void render(float alpha) override;
  void requestState(PlayerState &state);

  void useSkill(const std::string &skillname);
  void addSkill(const std::string &name, std::unique_ptr<ISkill> skill);

  // Actions
  void onMoveRight();
  void onMoveLeft();
  void onJump();
  void onStopLeft();
  void onStopRight();
  void onCrouch();
  void onAttack();

  // helper functions
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

  // Polymorphic Hook Methods override from Entity
  void onLand(float floorY) override;
  void onHitCeiling(float ceilY) override;
  void updateStateFromPhysics() override;

  // list of States
  PlayerIdleState idleState;
  PlayerRunState runState;
  PlayerJumpState jumpState;
  PlayerFallState fallState;
  PlayerCrouchState crouchState;
  PlayerHurtState hurtState;
  PlayerDieState dieState;
  PlayerSkillState skillState;
  void playAnimation(const std::string &name);

private:
  void changeState(PlayerState &state);

  PlayerState *currentState;
  std::unordered_map<std::string, Animation> animationList;
  std::unordered_map<std::string, std::unique_ptr<ISkill>> skillList;
};