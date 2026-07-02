#include "PlayerFactory.h"
#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "DashSkill.h"

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

  CharacterStats stats{
      charData["name"].get<std::string>(),
      charData["maxHealth"].get<int>(),
      charData["maxMana"].get<int>(),
      charData["moveSpeed"].get<float>(),
      charData["maxSpeed"].get<float>(),
      charData["acceleration"].get<float>(),
      charData["jumpForce"].get<float>(),
      charData["gravityScale"].get<float>(),
      skillList,
      Animation(
          AssetManager::getInstance().getTexture(
              charData["animations"]["idle"]["texture"].get<std::string>()),
          charData["animations"]["idle"]["frameNum"].get<int>(),
          charData["animations"]["idle"]["frameTime"].get<float>()),
      Animation(
          AssetManager::getInstance().getTexture(
              charData["animations"]["run"]["texture"].get<std::string>()),
          charData["animations"]["run"]["frameNum"].get<int>(),
          charData["animations"]["run"]["frameTime"].get<float>()),
      Animation(
          AssetManager::getInstance().getTexture(
              charData["animations"]["jump"]["texture"].get<std::string>()),
          charData["animations"]["jump"]["frameNum"].get<int>(),
          charData["animations"]["jump"]["frameTime"].get<float>())};

  Vector2 boxsize = {16.0f, 16.0f};
  auto player = std::make_unique<Player>(stats, pos, boxsize, true);
  for (const std::string &skillName : skillList) {
    if (skillName == "Dash") {
        player->addSkill("Dash", std::make_unique<DashSkill>());
    } 
    else if (skillName == "Fireball") {
    }
  }
  return player;
}