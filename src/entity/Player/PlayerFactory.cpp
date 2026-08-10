#include "PlayerFactory.h"
#include "AssetManager.h"
#include "BlockSkill.h"
#include "DashSkill.h"
#include "Punch1Skill.h"
#include "Punch2Skill.h"
#include "Punch3Skill.h"
#include "Punch4Skill.h"
#include "LongAttackSkill.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ── Skill Registry (OCP-compliant: add new skills here, never touch the factory loop) ──
static const std::unordered_map<std::string, std::function<std::unique_ptr<ISkill>()>> kSkillRegistry = {
    { "Dash",       []{ return std::make_unique<DashSkill>(); } },
    { "Block",      []{ return std::make_unique<BlockSkill>(); } },
    { "Punch1",     []{ return std::make_unique<Punch1Skill>(); } },
    { "Punch2",     []{ return std::make_unique<Punch2Skill>(); } },
    { "Punch3",     []{ return std::make_unique<Punch3Skill>(); } },
    { "Punch4",     []{ return std::make_unique<Punch4Skill>(); } },
    { "LongAttack", []{ return std::make_unique<LongAttackSkill>(); } },
};

std::unique_ptr<Player> PlayerFactory::createPlayer(const std::string &charName,
                                                    Vector2 pos) {
  std::ifstream file("assets/config/characters.json");
  if (!file.is_open()) {
    std::cerr << "Khong the mo file JSON: assets/config/characters.json" << std::endl;
    return nullptr;
  }

  json jsonData;
  file >> jsonData;

  if (!jsonData.contains(charName)) {
    std::cerr << "Khong tim thay thong tin cho nhan vat: " << charName << std::endl;
    return nullptr;
  }

  auto &charData = jsonData[charName];

  // Collect skill names from JSON
  std::vector<std::string> skillNames;
  for (auto& [key, value] : charData["skills"].items()) {
    skillNames.push_back(key);
  }

  // Build base stats from JSON
  CharacterBaseStats bS;
  bS.name         = charData["name"].get<std::string>();
  bS.maxHealth    = charData["maxHealth"].get<int>();
  bS.maxMana      = charData["maxMana"].get<int>();
  bS.moveVelocity = charData["moveVelocity"].get<float>();
  bS.jumpVelocity = charData["jumpVelocity"].get<float>();
  bS.gravityScale = charData["gravityScale"].get<float>();
  bS.physicsBox   = { charData["physicsBox"]["w"].get<float>(),
                      charData["physicsBox"]["h"].get<float>() };
  bS.crouchBox    = { charData["crouchBox"]["w"].get<float>(),
                      charData["crouchBox"]["h"].get<float>() };

  // Build runtime stats
  CharacterRuntimeStats rS;
  rS.health     = bS.maxHealth;
  rS.mana       = bS.maxMana;
  rS.physicsBox = bS.physicsBox;  // Start with standing hitbox
  rS.velocity   = {0.0f, 0.0f};
  rS.isGrounded = false;

  // Build world stats
  CharacterWorldStats wS;
  wS.position      = pos;
  wS.isFacingRight = true;
  wS.animation     = nullptr;

  // Load all animations from JSON
  std::unordered_map<std::string, Animation> animations;
  for (auto& [animName, animData] : charData["animations"].items()) {
    animations.emplace(animName, Animation(
        AssetManager::getInstance().getTexture(animData["texture"].get<std::string>()),
        animData["frameNum"].get<int>(),
        animData["frameTime"].get<float>()
    ));
  }

  auto player = std::make_unique<Player>(bS, rS, wS, std::move(animations));

  // Create and configure skills via registry (OCP — no if-else chain)
  for (const std::string &skillName : skillNames) {
    auto it = kSkillRegistry.find(skillName);
    if (it == kSkillRegistry.end()) {
      std::cerr << "[PlayerFactory] Unknown skill: " << skillName << std::endl;
      continue;
    }

    std::unique_ptr<ISkill> skill = it->second();

    // Inject combat data from JSON into skill
    auto& skillJson = charData["skills"][skillName];
    int atk = skillJson.value("attack", 0);
    int def = skillJson.value("defense", 0);
    Rectangle box = {
        skillJson["box"].value("offsetX", 0.0f),
        skillJson["box"].value("offsetY", 0.0f),
        skillJson["box"]["w"].get<float>(),
        skillJson["box"]["h"].get<float>()
    };
    skill->setCombatData(atk, def, box);

    // Calculate timing from animation data
    float duration = 0.0f, hitStart = 0.0f, hitEnd = 0.0f;
    std::string animName = skill->getAnimationName();
    if (charData["animations"].contains(animName)) {
      int frameNum    = charData["animations"][animName]["frameNum"].get<int>();
      float frameTime = charData["animations"][animName]["frameTime"].get<float>();
      duration = frameNum * frameTime;

      int startFrame = skillJson.value("hitboxStartFrame", 0);
      int endFrame   = skillJson.value("hitboxEndFrame", frameNum);
      hitStart = startFrame * frameTime;
      hitEnd   = endFrame * frameTime;
    }
    skill->setDurationAndHitbox(duration, hitStart, hitEnd);

    player->addSkill(skillName, std::move(skill));
  }

  return player;
}
