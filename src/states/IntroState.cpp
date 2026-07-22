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

  // Draw mana display for each player
  for (size_t i = 0; i < entities.size(); ++i) {
    const auto &entity = entities[i];
    const auto &base = entity->getBaseStats();
    const auto &runtime = entity->getRuntimeStats();
    const auto &world = entity->getWorldStats();

    // Position the mana bar above the player's head
    float barWidth = 200.0f;   // 50 * 4
    float barHeight = 24.0f;   // 6 * 4
    float barX = world.position.x;
    float barY = world.position.y + 75.0f; // 

    // Player name
    int nameWidth = MeasureText(base.name.c_str(), 48);
    DrawText(base.name.c_str(), static_cast<int>(barX + barWidth / 2 - nameWidth / 2), static_cast<int>(barY - 64), 48, WHITE);

    // Mana text (e.g. "50 / 100")
    std::string manaText = std::to_string(runtime.mana) + " / " + std::to_string(base.maxMana);
    int textWidth = MeasureText(manaText.c_str(), 40);
    DrawText(manaText.c_str(), static_cast<int>(barX + barWidth / 2 - textWidth / 2), static_cast<int>(barY + barHeight + 8), 40, SKYBLUE);

    // Background bar (dark)
    DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), DARKGRAY);

    // Filled mana bar (blue)
    float manaRatio = (base.maxMana > 0) ? static_cast<float>(runtime.mana) / static_cast<float>(base.maxMana) : 0.0f;
    DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth * manaRatio), static_cast<int>(barHeight), BLUE);

    // Bar outline
    DrawRectangleLines(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), WHITE);
  }
}