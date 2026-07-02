#pragma once
#include "Entity.h"
#include "IEntityCommand.h"
#include "Player.h"

class JumpCommand : public IEntityCommand {
public:
  void Execute(Entity &entity) override { entity.jump(); }
};

class MoveRightCommand : public IEntityCommand {
public:
  void Execute(Entity &entity) override { entity.moveRight(); }
};

class MoveLeftCommand : public IEntityCommand {
public:
  void Execute(Entity &entity) override { entity.moveLeft(); }
};

class StopMoveCommand : public IEntityCommand {
public:
  void Execute(Entity &entity) override { entity.stopMove(); }
};

class UseSkillCommand : public IEntityCommand {
private:
  std::string skillName;

public:
  UseSkillCommand(const std::string &name) : skillName(name) {}
  void Execute(Entity &entity) override {
    Player *player = dynamic_cast<Player *>(&entity);
    if (player) {
      player->useSkill(skillName);
    }
  }
};