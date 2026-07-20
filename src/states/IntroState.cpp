#include "IntroState.h"
#include "Animation.h"
#include "AssetManager.h"
#include "Entity.h"
#include "Player.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

IntroState::IntroState() {
  std::ifstream file("assets/config/session.json");
  if (!file.is_open()) {
    std::cerr << "Khong the mo file session.json. Su dung mac dinh.\n";
    exit(0);
  }

  json sessionData;
  file >> sessionData;

  for (const auto &playerInfo : sessionData["players"]) {
    std::string charName = playerInfo["character"].get<std::string>();
    Vector2 startPos = {playerInfo["start_pos"]["x"].get<float>(),
                        playerInfo["start_pos"]["y"].get<float>()};

    auto player = PlayerFactory::createPlayer(charName, startPos);
    if (player) {
      int jumpKey = playerInfo["controls"]["jump"].get<int>();
      int leftKey = playerInfo["controls"]["left"].get<int>();
      int rightKey = playerInfo["controls"]["right"].get<int>();
      int skill1Key = playerInfo["controls"]["skill1"].get<int>();
      int skill2Key = playerInfo["controls"]["skill2"].get<int>();
      InputHandler handler;
      handler.bindKey(jumpKey, std::make_unique<JumpCommand>(), true);
      handler.bindKey(leftKey, std::make_unique<MoveLeftCommand>(), true);
      handler.bindKey(rightKey, std::make_unique<MoveRightCommand>(), true);
      handler.bindKey(leftKey, std::make_unique<StopLeftCommand>(), false);
      handler.bindKey(rightKey, std::make_unique<StopRightCommand>(), false);

      // Bind skill keys
      handler.bindKey(skill1Key, std::make_unique<UseSkillCommand>("Dash"), true);
      handler.bindKey(skill2Key, std::make_unique<UseSkillCommand>("Punch1"), true);

      Player* playerPtr = player.get();
      entities.push_back(std::move(player));
      
      controllers.push_back({std::move(handler), playerPtr});
    }
  }
}

IntroState::~IntroState() = default;

void IntroState::HandleInput() {
  for (auto &controller : controllers) {
    if (controller.target) {
      auto commands = controller.handler.handleInput();
      for (auto *cmd : commands) {
        cmd->Execute(*(controller.target));
      }
    }
  }
}

void IntroState::Process() {
  float dt = GetFrameTime();
  float groundY = 500.0f;

  for (auto& entity : entities) {
    entity->applyGravity(dt);
    entity->updatePosition(dt);
    entity->checkGroundCollision(groundY);
    entity->updateStateFromPhysics();
  }
}

void IntroState::Update(float dt) {
  for (const auto &entity : entities) {
    entity->update(dt);
  }
}

void IntroState::Render(float alpha) const {
  for (const auto &entity : entities) {
    entity->render(alpha);
  }
}