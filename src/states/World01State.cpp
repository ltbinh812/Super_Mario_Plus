#include "World01State.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include "EntityFactory.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>

World01State::World01State() : mapCamera(416.0f) {
  bool loaded = map.LoadLDtkMap("assets/maps/map01/world01.ldtk", "Level_0");
  if (loaded && map.GetHeight() > 0) {
    std::cout << "[World01State] Da tai ban do map01 (Level_0) thanh cong!\n";

    // Khởi tạo Player 1 (Goku) tại {180.0f, 208.0f}
    player1 = PlayerFactory::createPlayer("Goku", {180.0f, 208.0f});
    if (player1) {
      player1->setCommandQueue(&spawnQueue);
      std::cout << "[World01State] Da them Player 1 (Goku)!\n";
      player1Handler.bindKey(KEY_A, std::make_unique<MoveLeftCommand>(), true);
      player1Handler.bindKey(KEY_D, std::make_unique<MoveRightCommand>(), true);
      player1Handler.bindKey(KEY_W, std::make_unique<ClimbCommand>(), true);
      player1Handler.bindKey(KEY_S, std::make_unique<CrouchCommand>(), true);
      player1Handler.bindKey(KEY_A, std::make_unique<StopLeftCommand>(), false);
      player1Handler.bindKey(KEY_D, std::make_unique<StopRightCommand>(), false);
      player1Handler.bindKey(KEY_J, std::make_unique<AttackCommand>(), true);
      player1Handler.bindKey(KEY_K, std::make_unique<JumpCommand>(), true);
      player1Handler.bindKey(KEY_L, std::make_unique<UseSkillCommand>("Dash"), true);
      
      
    }


    // Khởi tạo Player 2 (Luffy) tại {220.0f, 208.0f}
    player2 = PlayerFactory::createPlayer("Goku", {220.0f, 208.0f});
    if (player2) {
      player2->setCommandQueue(&spawnQueue);
      std::cout << "[World01State] Da them Player 2 (Luffy)!\n";
      player2Handler.bindKey(KEY_LEFT, std::make_unique<MoveLeftCommand>(), true);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<MoveRightCommand>(), true);
      player2Handler.bindKey(KEY_UP, std::make_unique<ClimbCommand>(), true);
      player2Handler.bindKey(KEY_DOWN, std::make_unique<CrouchCommand>(), true);
      player2Handler.bindKey(KEY_LEFT, std::make_unique<StopLeftCommand>(), false);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<StopRightCommand>(), false);
      player2Handler.bindKey(KEY_COMMA, std::make_unique<AttackCommand>(), true);
      player2Handler.bindKey(KEY_PERIOD, std::make_unique<JumpCommand>(), true);
      player2Handler.bindKey(KEY_SLASH, std::make_unique<UseSkillCommand>("Dash"), true);
    }

    // Register players with CombatSystem (one-way: CombatSystem observes entities)
    if (player1) combatSystem.registerEntity(player1.get());
    if (player2) combatSystem.registerEntity(player2.get());

  } else {
    std::cerr << "[World01State] Loi khi tai ban do map01!\n";
  }
}

World01State::~World01State() = default;

void World01State::HandleInput() {
  float dt = GetFrameTime();

  // Xử lý phím điều khiển cho Player 1
  if (player1) {
    auto commands = player1Handler.handleInput();
    for (auto *cmd : commands) {
      cmd->Execute(*player1);
    }
  }

  // Xử lý phím điều khiển cho Player 2
  if (player2) {
    auto commands = player2Handler.handleInput();
    for (auto *cmd : commands) {
      cmd->Execute(*player2);
    }
  }

  // Cập nhật camera chuẩn hoá bám theo trung điểm của 2 nhân vật (Dynamic Co-op Camera)
  if (player1 && player2) {
    mapCamera.UpdateMultiplayer(player1->getWorldStats().position, player2->getWorldStats().position,
                                map.GetWidth(), map.GetHeight(), dt);
  } else if (player1) {
    mapCamera.Update(player1->getWorldStats().position, map.GetWidth(), map.GetHeight(), dt);
  } else if (player2) {
    mapCamera.Update(player2->getWorldStats().position, map.GetWidth(), map.GetHeight(), dt);
  }
}

void World01State::Process() {
  // Hàm này để trống hoặc dùng để xử lý Command (nếu có logic phụ thêm)
}

void World01State::Update(float dt) {
  // Cập nhật Thế giới tự nhiên: Vật lý, Va chạm, Hoạt ảnh, v.v.
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

  for (auto& entity : activeEntities) {
      entity->updatePhysicsWithMap(map, dt);
      entity->update(dt);
  }

  auto commands = spawnQueue.popAll();
  for (const auto& cmd : commands) {
      auto entity = EntityFactory::create(cmd);
      if (entity) {
          entity->setCommandQueue(&spawnQueue);
          combatSystem.registerEntity(entity.get());
          activeEntities.push_back(std::move(entity));
      }
  }

  activeEntities.erase(std::remove_if(activeEntities.begin(), activeEntities.end(), 
      [](const std::unique_ptr<Entity>& e) { return !e->getIsActive(); }), activeEntities.end());
  combatSystem.removeInactive();

  // Combat: polls entities for active hitboxes, detects collisions, applies damage
  combatSystem.update(dt);
}

void World01State::Render(float alpha) const {
  ClearBackground(BLACK); // Nền đen mặc định cho phần không nhìn thấy của map

  mapCamera.BeginMode();
  map.Draw();
  if (player1) {
    player1->render(alpha);
  }
  if (player2) {
    player2->render(alpha);
  }
  for (const auto& entity : activeEntities) {
      entity->render(alpha);
  }
  combatSystem.renderDebug();  // Debug: green = attack hitbox, blue = defense hitbox
  mapCamera.EndMode();

  // Debug UI overlays
  DrawText("WORLD 01 STATE - CO-OP MULTIPLAYER & DYNAMIC ZOOM CAMERA", 10, 10,
           20, YELLOW);
  DrawText(
      "P1 (Goku): A/D Move | J: Punch | K: Jump | L: Dash",
      10, 35, 20, WHITE);
  DrawText(
      "P2 (Luffy): LEFT/RIGHT Move | ,: Punch | .: Jump | /: Dash",
      10, 60, 20, WHITE);
  Vector2 camTarget = mapCamera.GetTarget();
  DrawText(("Camera Target: (" + std::to_string((int)camTarget.x) + ", " +
            std::to_string((int)camTarget.y) +
            ") | Zoom: " + std::to_string(mapCamera.GetZoom()))
               .c_str(),
           10, 85, 20, LIGHTGRAY);
  if (player1 && player2) {
    DrawText("Dynamic Co-op Camera active: smoothly zooms out when players separate!",
             10, 110, 20, GREEN);
  }
}
