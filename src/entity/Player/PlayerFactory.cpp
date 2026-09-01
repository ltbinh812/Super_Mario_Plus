#include "PlayerFactory.h"
#include "AssetManager.h"
#include "BlockSkill.h"
#include "DashSkill.h"
#include "Attack1Skill.h"
#include "Attack2Skill.h"
#include "Attack3Skill.h"
#include "Attack4Skill.h"
#include "LongAttackSkill.h"
#include "JumpAttackSkill.h"
#include "LowAttackSkill.h"
#include "SpecialSkillAttack.h"
#include <fstream>
#include <functional>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ── Skill Registry (OCP-compliant: add new skills here, never touch the factory loop) ──
static const std::unordered_map<std::string, std::function<std::unique_ptr<ISkill>()>> kSkillRegistry = {
    { "Dash",       []{ return std::make_unique<DashSkill>(); } },
    { "Block",      []{ return std::make_unique<BlockSkill>(); } },
    { "Attack1",    []{ return std::make_unique<Attack1Skill>(); } },
    { "Attack2",    []{ return std::make_unique<Attack2Skill>(); } },
    { "Attack3",    []{ return std::make_unique<Attack3Skill>(); } },
    { "Attack4",    []{ return std::make_unique<Attack4Skill>(); } },
    { "LongAttack", []{ return std::make_unique<LongAttackSkill>(); } },
    { "JumpAttack", []{ return std::make_unique<JumpAttackSkill>(); } },
    { "LowAttack",  []{ return std::make_unique<LowAttackSkill>(); } },
    { "SpecialAttack", []{ return std::make_unique<SpecialSkillAttack>(); } },
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

  // Resolve asset folder for auto-loading textures
  std::string assetFolder = charData["assetFolder"].get<std::string>();
  std::string charDisplayName = charData["name"].get<std::string>();

  // Load all animations from JSON (auto-load textures from assetFolder)
  std::unordered_map<std::string, Animation> animations;
  for (auto& [animName, animData] : charData["animations"].items()) {
    std::string texBase = animData["texture"].get<std::string>();
    std::string texKey  = charDisplayName + "_" + texBase;
    std::string texPath = "assets/" + assetFolder + "/" + texBase + ".png";

    // Load texture if not already loaded
    AssetManager::getInstance().loadTexture(texKey, texPath);
    
    // Load sound for this animation if exists
    std::string soundPath = "assets/" + assetFolder + "/sounds/" + animName + ".wav";
    std::string soundKey  = charDisplayName + "_" + animName + "_sound";
    if (std::filesystem::exists(soundPath)) {
        AssetManager::getInstance().loadSound(soundKey, soundPath);
    }

    float scale = animData.value("scale", 1.0f);

    animations.emplace(animName, Animation(
        AssetManager::getInstance().getTexture(texKey),
        animData["frameNum"].get<int>(),
        animData["frameTime"].get<float>(),
        scale
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
    float manaCost = skillJson.value("manaCost", skill->getManaCost()); // fallback to default cost if not in JSON
    skill->setManaCost(manaCost);
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
    
    // Inject pacing data from JSON into skill
    float recovery = skillJson.value("recoveryDuration", 0.1f);
    float hitstop = skillJson.value("hitStopDuration", 0.05f);
    float anticipation = skillJson.value("anticipationDuration", 0.0f);
    skill->setPacingData(recovery, hitstop, anticipation);

    float moveControl = skillJson.value("moveControl", 0.0f);
    skill->setMoveControl(moveControl);

    float dashMultiplier = skillJson.value("dashMultiplier", 2.0f);
    skill->setDashMultiplier(dashMultiplier);

    player->addSkill(skillName, std::move(skill));
  }

  // NEW: Parse soundFrames for Animation Events
  if (charData.contains("soundFrames")) {
      std::unordered_map<std::string, std::unordered_map<int, std::string>> soundFrames;
      for (auto& [animName, frameData] : charData["soundFrames"].items()) {
          for (auto& [frameIdxStr, soundSuffix] : frameData.items()) {
              int frameIdx = std::stoi(frameIdxStr);
              std::string suffix = soundSuffix.get<std::string>();
              
              soundFrames[animName][frameIdx] = suffix;
              
              std::string assetFolder = charData.value("assetFolder", "goku");
              std::string soundPath = "assets/" + assetFolder + "/sounds/" + suffix + ".wav";
              std::string soundKey = charName + "_" + suffix + "_sound";
              std::cout << "[DEBUG-FACTORY] Attempting to load sound: " << soundKey << " from " << soundPath << std::endl;
              if (std::filesystem::exists(soundPath)) {
                  std::cout << "[DEBUG-FACTORY] Path exists! Loading..." << std::endl;
                  AssetManager::getInstance().loadSound(soundKey, soundPath);
              } else {
                  std::cout << "[ERROR-FACTORY] Path does NOT exist: " << soundPath << std::endl;
              }
          }
      }
      player->setSoundFrames(std::move(soundFrames));
  }

  return player;
}

