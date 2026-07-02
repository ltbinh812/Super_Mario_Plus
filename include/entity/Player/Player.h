#pragma once
#include "CharacterStats.h"
#include "Entity.h"
#include "IEntityState.h"
#include "InputHandler.h"
#include "PlayerStates.h"
#include "SkillManager.h"

class Player : public Entity {
private:
  bool isFacingRight;
  bool isGrounded;
  float airSpeed;

  bool isMovingLeft = false;
  bool isMovingRight = false;

  IEntityState<Player> *currentState;
  PlayerIdleState idleState;
  PlayerRunState runState;
  PlayerJumpState jumpState;
  IEntityState<Player> *requestState = nullptr;

  CharacterStats stats;
  SkillManager skillManager;

public:
  Player(const CharacterStats &charStats, Vector2 pos, Vector2 boxsize, bool isRight);

  void handleInput() override;
  void process() override;
  void update(float dt) override;
  void render(float alpha) const override;

  void jump() override;
  void moveRight() override;
  void moveLeft() override;
  void stopMove() override;

  std::string getSkill1Name();
  std::string getSkill2Name();
  void addSkill(const std::string& name, std::unique_ptr<ISkill> skill);
  void useSkill(const std::string &skillName);

  void setRequest(IEntityState<Player> *state);
  void processRequest();
  void changeState(IEntityState<Player> *state);
  void setAnimation(Animation *anim);

  IEntityState<Player> *getIdleState() { return &idleState; }
  IEntityState<Player> *getRunState() { return &runState; }
  IEntityState<Player> *getJumpState() { return &jumpState; }

  Animation *getIdleAnimation() { return &stats.idleAnimation; }
  Animation *getRunAnimation() { return &stats.runAnimation; }
  Animation *getJumpAnimation() { return &stats.jumpAnimation; }
  const CharacterStats &getStats() const { return stats; }

  void setFaceDirection(bool isRight);
  bool getFaceDirection() const { return isFacingRight; }
  void setVelocityX(float vel);
  void setVelocityY(float vel);
  bool checkIsGrounded();
  void setIsGrounded(bool grounded);
};