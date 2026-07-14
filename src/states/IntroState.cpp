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
  // physics system
  float dt = GetFrameTime();
  for (auto& entity : entities) {
    auto& runtime = entity->getRuntimeStats();
    auto& world = entity->getWorldStats();
    auto& base = entity->getBaseStats();

    // Apply gravity
    runtime.velocity.y += base.gravityScale * 9.8f * dt;

    // Update position
    world.position.x += runtime.velocity.x * dt;
    world.position.y += runtime.velocity.y * dt;

    // Simple ground check
    float groundY = 500.0f;
    if (world.position.y >= groundY) {
      world.position.y = groundY;
      runtime.velocity.y = 0.0f;
      runtime.isGrounded = true;
    } else {
      runtime.isGrounded = false;
    }

    // State check for fall/grounded
    if (!runtime.isGrounded) {
      if (runtime.velocity.y > 0) {
        entity->changeState(entity->fallState);
      } else {
        entity->changeState(entity->jumpState);
      }
    } else {
      if (runtime.velocity.x == 0.0f) {
        entity->changeState(entity->idleState);
      } else {
        entity->changeState(entity->runState);
      }
    }
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