#include "World01State.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>

World01State::World01State() : cameraX(0.0f), cameraY(0.0f), zoom(2.0f) {
  bool loaded = map.LoadLDtkMap("assets/maps/map01/world01.ldtk", "Level_0");
  if (loaded && map.GetHeight() > 0) {
    std::cout << "[World01State] Da tai ban do map01 (Level_0) thanh cong!\n";
    // 1. CỐ ĐỊNH TỶ LỆ ZOOM: Sử dụng chiều cao tầm nhìn chuẩn (Viewport Height
    // = 416px, tương đương 26 ô gạch 16x16). Nhờ cơ chế Fixed Vertical Viewport
    // này, dù tương lai bạn tạo map leo tháp dọc (cao 3000px) hay open world
    // ngang (rộng 5000px), kích thước nhân vật (1 block) và các ô gạch trên màn
    // hình luôn giữ chuẩn nguyên vẹn 100% không bao giờ bị méo hay nhỏ lại!
    constexpr float FIXED_VIEWPORT_HEIGHT = 416.0f;
    zoom = (float)GetScreenHeight() / FIXED_VIEWPORT_HEIGHT;

    // 2. Đặt vị trí khởi đầu map ở bên trái cùng của map (X = viewW / 2)
    // 2. Đặt bản đồ ở tọa độ 0,0 góc trái trên cùng màn hình
    float viewW = GetScreenWidth() / zoom;
    float viewH = GetScreenHeight() / zoom;
    cameraX = viewW / 2.0f;
    cameraY = viewH / 2.0f;

    // 3. Thêm 1 Player (Goku) tại vị trí bãi cỏ bên trái cùng của map (X = 200,
    // Y = 208)
    testPlayer = PlayerFactory::createPlayer("Goku", {200.0f, 208.0f});
    if (testPlayer) {
      std::cout
          << "[World01State] Da them testPlayer (Goku) de khao sat ty le!\n";
      // Gán phím điều khiển cho nhân vật
      playerHandler.bindKey(KEY_A, std::make_unique<MoveLeftCommand>(), true);
      playerHandler.bindKey(KEY_D, std::make_unique<MoveRightCommand>(), true);
      playerHandler.bindKey(KEY_LEFT, std::make_unique<MoveLeftCommand>(),
                            true);
      playerHandler.bindKey(KEY_RIGHT, std::make_unique<MoveRightCommand>(),
                            true);
      playerHandler.bindKey(KEY_A, std::make_unique<StopLeftCommand>(), false);
      playerHandler.bindKey(KEY_D, std::make_unique<StopRightCommand>(), false);
      playerHandler.bindKey(KEY_LEFT, std::make_unique<StopLeftCommand>(),
                            false);
      playerHandler.bindKey(KEY_RIGHT, std::make_unique<StopRightCommand>(),
                            false);
      playerHandler.bindKey(KEY_J, std::make_unique<UseSkillCommand>("Punch1"),
                            true);
      playerHandler.bindKey(KEY_K, std::make_unique<UseSkillCommand>("Dash"),
                            true);
    }
  } else {
    std::cerr << "[World01State] Loi khi tai ban do map01!\n";
  }
}

World01State::~World01State() = default;

void World01State::HandleInput() {
  float dt = GetFrameTime();

  // TẤT bộ di chuyển camera thủ công bằng W/A/S/D
  /*
  float speed = 400.0f * dt / zoom;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) cameraX += speed;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) cameraX -= speed;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) cameraY += speed;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) cameraY -= speed;
  */

  // BẬT bộ điều khiển di chuyển cho nhân vật (A/D hoặc mũi tên Trái/Phải để di
  // chuyển, J đấm, K lướt)
  if (testPlayer) {
    auto commands = playerHandler.handleInput();
    for (auto *cmd : commands) {
      cmd->Execute(*testPlayer);
    }
  }

  // TẤT zoom thủ công (Q/E) và giữ vững tỷ lệ khung hình chuẩn 416px cho mọi
  // level (ngang lẫn dọc)
  constexpr float FIXED_VIEWPORT_HEIGHT = 416.0f;
  zoom = (float)GetScreenHeight() / FIXED_VIEWPORT_HEIGHT;

  float viewW = GetScreenWidth() / zoom;
  float viewH = GetScreenHeight() / zoom;

  // Với map đi ngang này: Camera chỉ di chuyển theo chiều ngang (cameraX) bám
  // theo nhân vật. Chiều dọc (cameraY) được đặt ở viewH / 2.0f để tọa độ Y = 0
  // luôn nằm chính xác ở góc trên cùng màn hình!
  if (testPlayer) {
    cameraX = testPlayer->getWorldStats().position.x;
    cameraY = viewH / 2.0f;
  }

  // Camera Clamping chiều ngang: Khóa không cho cạnh trái/phải camera vượt ra
  // ngoài ranh giới ngang bản đồ
  if (map.GetWidth() > 0 && map.GetHeight() > 0) {
    float minX = viewW / 2.0f;
    float maxX = std::max(minX, (float)map.GetWidth() - viewW / 2.0f);
    cameraX = std::clamp(cameraX, minX, maxX);
  }
}

void World01State::Process() {
  // Bật trọng lực và cập nhật vị trí cho nhân vật
  if (testPlayer) {
    float dt = GetFrameTime();
    testPlayer->applyGravity(dt);
    testPlayer->updatePosition(dt);
    // Dùng checkGroundCollision tại Y=208 để giữ nhân vật đứng vững trên mặt cỏ
    // ở giữa map
    testPlayer->checkGroundCollision(208.0f);
    testPlayer->updateStateFromPhysics();
  }
}

void World01State::Update(float dt) {
  if (testPlayer) {
    testPlayer->update(dt);
  }
}

void World01State::Render(float alpha) const {
  ClearBackground(BLACK); // Nền đen mặc định cho phần không nhìn thấy của map

  Camera2D camera = {0};
  camera.target = {cameraX, cameraY};
  camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = zoom;

  BeginMode2D(camera);
  map.Draw();
  if (testPlayer) {
    testPlayer->render(alpha);
  }
  EndMode2D();

  // Debug UI overlays
  DrawText("WORLD 01 STATE - FIXED VIEWPORT RATIO (416px ~ 26 TILES)", 10, 10,
           20, YELLOW);
  DrawText(
      "Use A/D or LEFT/RIGHT to Move Goku | J: Punch | K: Dash | W/SPACE: Jump",
      10, 35, 20, WHITE);
  DrawText(("Camera Pos: (" + std::to_string((int)cameraX) + ", " +
            std::to_string((int)cameraY) + ") | Zoom: " + std::to_string(zoom))
               .c_str(),
           10, 60, 20, LIGHTGRAY);
  if (testPlayer) {
    DrawText("Fixed Zoom: Preserves exact 1-block player & tile ratio on all "
             "future maps (H & V)!",
             10, 85, 20, GREEN);
  }
}
