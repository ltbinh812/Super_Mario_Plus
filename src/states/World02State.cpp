#include "World02State.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>

World02State::World02State() : mapCamera(416.0f) {
  bool loaded = map.LoadLDtkMap("assets/maps/map02/world02.ldtk", "");
  if (loaded && map.GetHeight() > 0) {
    std::cout
        << "[World02State] Da tai ban do map02 (Auto fallback) thanh cong!\n";

    // Khởi tạo Player 1 (Goku) tại {380.0f, 100.0f}
    player1 = PlayerFactory::createPlayer("Goku", {380.0f, 200.0f});
    if (player1) {
      std::cout << "[World02State] Da them Player 1 (Goku) vao map02!\n";
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


    // Khởi tạo Player 2 (Luffy) tại {420.0f, 100.0f}
    player2 = PlayerFactory::createPlayer("Luffy", {420.0f, 200.0f});
    if (player2) {
      std::cout << "[World02State] Da them Player 2 (Luffy) vao map02!\n";
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
    std::cerr << "[World02State] Loi khi tai ban do map02!\n";
  }
}

World02State::~World02State() = default;

void World02State::HandleInput() {
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

  // Cập nhật camera co-op bám theo trung điểm 2 nhân vật
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

void World02State::Process() {
  // Hàm này để trống hoặc dùng để xử lý Command (nếu có logic phụ thêm)
}

void World02State::Update(float dt) {
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
}

void World02State::Render(float alpha) const {
  ClearBackground(DARKGRAY); // Nền tối cho hang động cavern

  mapCamera.BeginMode();
  map.Draw();
  if (player1) {
    player1->render(alpha);
  }
  if (player2) {
    player2->render(alpha);
  }
  mapCamera.EndMode();

  // Debug UI overlays
  DrawText("WORLD 02 STATE - CO-OP MULTIPLAYER & DYNAMIC ZOOM CAMERA", 10, 10,
           20, YELLOW);
  DrawText("P1 (Goku): A/D Move | J: Punch | K: Jump | L: Dash", 10, 35, 20,
           WHITE);
  DrawText("P2 (Luffy): LEFT/RIGHT Move | ,: Punch | .: Jump | /: Dash", 10, 60,
           20, WHITE);
  Vector2 camTarget = mapCamera.GetTarget();
  DrawText(("Camera Target: (" + std::to_string((int)camTarget.x) + ", " +
            std::to_string((int)camTarget.y) +
            ") | Zoom: " + std::to_string(mapCamera.GetZoom()))
               .c_str(),
           10, 85, 20, LIGHTGRAY);
  if (player1 && player2) {
    DrawText("Dynamic Co-op Camera active: smoothly zooms out when players "
             "separate!",
             10, 110, 20, GREEN);
  }
}
