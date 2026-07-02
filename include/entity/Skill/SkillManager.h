#pragma once
#include "ISkill.h"
#include <memory>
#include <string>
#include <unordered_map>


class Player;

class SkillManager {
private:
  std::unordered_map<std::string, std::unique_ptr<ISkill>> listSkills;

public:
  void addSkill(const std::string &name, std::unique_ptr<ISkill> skill);

  void update(float dt);

  bool useSkill(const std::string &name, Player &player);
};