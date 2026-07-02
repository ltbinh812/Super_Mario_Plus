#include "SkillManager.h"
#include "entity/Player/Player.h"

void SkillManager::addSkill(const std::string &name, std::unique_ptr<ISkill> skill) {
  listSkills[name] = std::move(skill);
}

void SkillManager::update(float dt) {
  for (auto &pair : listSkills) {
    pair.second->update(dt);
  }
}

bool SkillManager::useSkill(const std::string &name, Player &player) {
  auto it = listSkills.find(name);
  if (it != listSkills.end() && it->second->isReady()) {
    it->second->execute(player);
    it->second->startCooldown();
    return true;
  }
  return false;
}