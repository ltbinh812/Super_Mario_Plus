#include "PlayerFactory.h"
#include "AssetManager.h"
#include "DashSkill.h"
#include "PunchSkill.h"
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
  for (auto &s : charData["skills"]) {
    skillList.push_back(s.get<std::string>());
  }

  CharacterBaseStats bS;
  bS.name = charData["name"].get<std::string>();
  bS.maxHealth = charData["maxHealth"].get<int>();
  bS.maxMana = charData["maxMana"].get<int>();
  bS.moveVelocity = charData["moveVelocity"].get<float>();
  bS.jumpVelocity = charData["jumpVelocity"].get<float>();
  bS.gravityScale = charData["gravityScale"].get<float>();

  CharacterRuntimeStats rS;
  rS.health = bS.maxHealth;
  rS.mana = bS.maxMana;
  rS.hitbox = {16.0f, 16.0f};
  rS.velocity = {0.0f, 0.0f};
  rS.isGrounded = false;

  CharacterWorldStats wS;
  wS.position = pos;
  wS.isFacingRight = true;
  wS.animation = nullptr;

  std::unordered_map<std::string, Animation> animations;
  
  auto addAnimation = [&](const std::string& animName) {
      if (charData["animations"].contains(animName)) {
          auto& animData = charData["animations"][animName];
          animations.emplace(animName, Animation(
              AssetManager::getInstance().getTexture(animData["texture"].get<std::string>()),
              animData["frameNum"].get<int>(),
              animData["frameTime"].get<float>()
          ));
      }
  };

  addAnimation("idle");
  addAnimation("run");
  addAnimation("jump");
  addAnimation("fall");
  addAnimation("crouch");
  addAnimation("hurt");
  addAnimation("die");
  addAnimation("dash");
  addAnimation("punch1");

  auto player = std::make_unique<Player>(bS, rS, wS, std::move(animations));

  for (const std::string &skillName : skillList) {
    if (skillName == "Dash") {
        player->addSkill("Dash", std::make_unique<DashSkill>());
    } 
    else if (skillName == "Punch1") {
        player->addSkill("Punch1", std::make_unique<PunchSkill>());
    }
  }
  return player;
}