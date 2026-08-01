#include "PlayerFactory.h"
#include "AssetManager.h"
#include "BlockSkill.h"
#include "DashSkill.h"
#include "Punch1Skill.h"
#include "Punch2Skill.h"
#include "Punch3Skill.h"
#include "Punch4Skill.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

std::unique_ptr<Player> PlayerFactory::createPlayer(const std::string &charName,
                                                    Vector2 pos) {
  std::ifstream file("assets/config/characters.json");
  if (!file.is_open()) {
    std::cerr << "Khong the mo file JSON: assets/config/characters.json"
              << std::endl;
    return nullptr;
  }

  json jsonData;
  file >> jsonData;

  if (!jsonData.contains(charName)) {
    std::cerr << "Khong tim thay thong tin cho nhan vat: " << charName
              << std::endl;
    return nullptr;
  }

  auto &charData = jsonData[charName];

  std::vector<std::string> skillList;
  for (auto& [key, value] : charData["skills"].items()) {
    skillList.push_back(key);
  }

  CharacterBaseStats bS;
  bS.name = charData["name"].get<std::string>();
  bS.maxHealth = charData["maxHealth"].get<int>();
  bS.maxMana = charData["maxMana"].get<int>();
  bS.moveVelocity = charData["moveVelocity"].get<float>();
  bS.jumpVelocity = charData["jumpVelocity"].get<float>();
  bS.gravityScale = charData["gravityScale"].get<float>();
  bS.physicsBox = {
      charData["physicsBox"]["w"].get<float>(),
      charData["physicsBox"]["h"].get<float>()
  };
  bS.crouchBox = {
      charData["crouchBox"]["w"].get<float>(),
      charData["crouchBox"]["h"].get<float>()
  };

  CharacterRuntimeStats rS;
  rS.health = bS.maxHealth;
  rS.mana = bS.maxMana;
  rS.physicsBox = bS.physicsBox;
  rS.velocity = {0.0f, 0.0f};
  rS.isGrounded = false;

  CharacterWorldStats wS;
  wS.position = pos;
  wS.isFacingRight = true;
  wS.animation = nullptr;

  std::unordered_map<std::string, Animation> animations;

  for (auto& [animName, animData] : charData["animations"].items()) {
      animations.emplace(animName, Animation(
          AssetManager::getInstance().getTexture(animData["texture"].get<std::string>()),
          animData["frameNum"].get<int>(),
          animData["frameTime"].get<float>()
      ));
  }

  auto player = std::make_unique<Player>(bS, rS, wS, std::move(animations));

  for (const std::string &skillName : skillList) {
    std::unique_ptr<ISkill> skill;

    if (skillName == "Dash") {
        skill = std::make_unique<DashSkill>();
    } else if (skillName == "Block") {
        skill = std::make_unique<BlockSkill>();
    } else if (skillName == "Punch1") {
        skill = std::make_unique<Punch1Skill>();
    } else if (skillName == "Punch2") {
        skill = std::make_unique<Punch2Skill>();
    } else if (skillName == "Punch3") {
        skill = std::make_unique<Punch3Skill>();
    } else if (skillName == "Punch4") {
        skill = std::make_unique<Punch4Skill>();
    } else {
        std::cerr << "Unknown skill: " << skillName << std::endl;
        continue;
    }

    // Load combat data from JSON and inject into skill
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

    player->addSkill(skillName, std::move(skill));
  }
  return player;
}