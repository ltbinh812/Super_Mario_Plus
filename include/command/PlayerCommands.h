#pragma once
#include "IPlayerCommand.h"
#include "Player.h"

class JumpCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onJump();
  }
};

class MoveRightCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onMoveRight();
  }
};

class MoveLeftCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onMoveLeft();
  }
};

class StopLeftCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onStopLeft();
  }
};

class StopRightCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onStopRight();
  }
};

class CrouchCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onCrouch();
  }
};

class StopCrouchCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onStopCrouch();
  }
};

class AttackCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onAttack();
  }
};


class UseSkillCommand : public IPlayerCommand {
private:
  std::string skillName;

public:
  UseSkillCommand(const std::string &name) : skillName(name) {}
  void Execute(Player &player) override {
    player.useSkill(skillName);
  }
};

class DropThroughCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.dropThrough();
  }
};

class ClimbCommand : public IPlayerCommand {
public:
  void Execute(Player &player) override {
    player.onClimb();
  }
};

