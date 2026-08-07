#include "World03State.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>

World03State::World03State() : mapCamera(416.0f) {
  currentLevel =
      "Your_typical_2D_platformer"; // Default map03 level based on JSON
  bool loaded = map.LoadLDtkMap("assets/maps/map03/world03.ldtk", currentLevel);
  if (loaded && map.GetHeight() > 0) {
    std::cout << "[World03State] Da tai ban do map03 (" << currentLevel
              << ") thanh cong!\n";

    // Setup Player 1
    player1 = PlayerFactory::createPlayer("Goku", {260.0f, 208.0f});
    if (player1) {
      player1Handler.bindKey(KEY_A, std::make_unique<MoveLeftCommand>(), true);
      player1Handler.bindKey(KEY_D, std::make_unique<MoveRightCommand>(), true);
      player1Handler.bindKey(KEY_W, std::make_unique<ClimbCommand>(), true);
      player1Handler.bindKey(KEY_S, std::make_unique<CrouchCommand>(), true);
      player1Handler.bindKey(KEY_A, std::make_unique<StopLeftCommand>(), false);
      player1Handler.bindKey(KEY_D, std::make_unique<StopRightCommand>(),
                             false);
      player1Handler.bindKey(KEY_J, std::make_unique<UseSkillCommand>("Punch1"),
                             true);

      player1Handler.bindKey(KEY_K, std::make_unique<JumpCommand>(), true);
      player1Handler.bindKey(KEY_L, std::make_unique<UseSkillCommand>("Dash"),
                             true);
    }

    // Setup Player 2
    player2 = PlayerFactory::createPlayer("Luffy", {260.0f, 208.0f});
    if (player2) {
      player2Handler.bindKey(KEY_LEFT, std::make_unique<MoveLeftCommand>(),
                             true);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<MoveRightCommand>(),
                             true);
      player2Handler.bindKey(KEY_UP, std::make_unique<ClimbCommand>(), true);
      player2Handler.bindKey(KEY_DOWN, std::make_unique<CrouchCommand>(), true);
      player2Handler.bindKey(KEY_LEFT, std::make_unique<StopLeftCommand>(),
                             false);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<StopRightCommand>(),
                             false);
      player2Handler.bindKey(KEY_COMMA,
                             std::make_unique<UseSkillCommand>("Punch1"), true);

      player2Handler.bindKey(KEY_PERIOD, std::make_unique<JumpCommand>(), true);
      player2Handler.bindKey(KEY_SLASH,
                             std::make_unique<UseSkillCommand>("Dash"), true);
    }
  } else {
    std::cerr << "[World03State] Loi khi tai ban do map03!\n";
  }
}

World03State::~World03State() = default;

void World03State::HandleInput() {
  float dt = GetFrameTime();
  if (player1) {
    auto commands = player1Handler.handleInput();
    for (auto *cmd : commands)
      cmd->Execute(*player1);
  }
  if (player2) {
    auto commands = player2Handler.handleInput();
    for (auto *cmd : commands)
      cmd->Execute(*player2);
  }

  if (player1 && player2) {
    mapCamera.UpdateMultiplayer(player1->getWorldStats().position,
                                player2->getWorldStats().position,
                                map.GetWidth(), map.GetHeight(), dt);
  } else if (player1) {
    mapCamera.Update(player1->getWorldStats().position, map.GetWidth(),
                     map.GetHeight(), dt);
  } else if (player2) {
    mapCamera.Update(player2->getWorldStats().position, map.GetWidth(),
                     map.GetHeight(), dt);
  }
}

void World03State::Process() {}

void World03State::Update(float dt) {
  if (player1) {
    player1->updatePhysicsWithMap(map, dt);
    player1->updateStateFromPhysics();
    player1->update(dt);
  }
  if (player2) {
    player2->updatePhysicsWithMap(map, dt);
    player2->updateStateFromPhysics();
    player2->update(dt);
  }

  // Check for level transitions (both players must cross)
  if (player1 && player2) {
    float p1X = player1->getWorldStats().position.x;
    float p2X = player2->getWorldStats().position.x;
    float p1Y = player1->getWorldStats().position.y;
    float p2Y = player2->getWorldStats().position.y;

    int mapW = map.GetWidth();
    int mapH = map.GetHeight();

    float edge = 16.0f; // 0.5 block
    
    std::string next = "";
    std::string dir = "";
    
    float triggerLocalX = 0;
    float triggerLocalY = 0;

    if (p1X > mapW - edge || p2X > mapW - edge) {
      dir = "e";
      triggerLocalX = (p1X > p2X) ? p1X : p2X;
      triggerLocalY = (p1X > p2X) ? p1Y : p2Y;
    } else if (p1X < edge || p2X < edge) {
      dir = "w";
      triggerLocalX = (p1X < p2X) ? p1X : p2X;
      triggerLocalY = (p1X < p2X) ? p1Y : p2Y;
    } else if (p1Y > mapH - edge || p2Y > mapH - edge) {
      dir = "s";
      triggerLocalX = (p1Y > p2Y) ? p1X : p2X;
      triggerLocalY = (p1Y > p2Y) ? p1Y : p2Y;
    } else if (p1Y < edge || p2Y < edge) {
      dir = "n";
      triggerLocalX = (p1Y < p2Y) ? p1X : p2X;
      triggerLocalY = (p1Y < p2Y) ? p1Y : p2Y;
    }

    if (!dir.empty()) {
      float globalX = triggerLocalX + map.GetWorldX();
      float globalY = triggerLocalY + map.GetWorldY();
      next = map.GetNeighbour(dir, globalX, globalY);
      
      if (!next.empty()) {
        TransitionToLevel(next, dir, globalX, globalY);
      }
    }
  }
}

void World03State::TransitionToLevel(const std::string &nextLevel,
                                     const std::string &dir,
                                     float triggerGlobalX,
                                     float triggerGlobalY) {
  std::cout << "[World03State] Transitioning to " << nextLevel
            << " (dir: " << dir << ")\n";

  if (map.LoadLDtkMap("assets/maps/map03/world03.ldtk", nextLevel)) {
    currentLevel = nextLevel;
    int mapW = map.GetWidth();
    int mapH = map.GetHeight();
    int newWorldX = map.GetWorldX();
    int newWorldY = map.GetWorldY();

    float targetXNew = triggerGlobalX - newWorldX;
    float targetYNew = triggerGlobalY - newWorldY;

    // Adjust player positions based on direction
    float margin = 64.0f; // Triß╗çu hß╗ôi ß╗ƒ ├┤ block liß╗ün kß╗ü ─æß╗â kh├┤ng d├¡nh m├⌐p
    if (dir == "e") {
      targetXNew = margin;
    } else if (dir == "w") {
      targetXNew = mapW - margin;
    } else if (dir == "s") {
      targetYNew = margin;
    } else if (dir == "n") {
      targetYNew = mapH - margin;
    }

    player1->setPosition({targetXNew, targetYNew});
    player2->setPosition({targetXNew, targetYNew});
  } else {
    std::cerr << "[World03State] Failed to transition to level: " << nextLevel
              << "\n";
  }
}

void World03State::Render(float alpha) const {
  ClearBackground(BLACK);
  mapCamera.BeginMode();
  map.Draw();
  if (player1)
    player1->render(alpha);
  if (player2)
    player2->render(alpha);
  mapCamera.EndMode();

  DrawText("WORLD 03 STATE - LEVEL TRANSITIONS", 10, 10, 20, YELLOW);
  DrawText(("Current Level: " + currentLevel).c_str(), 10, 35, 20, GREEN);
}
