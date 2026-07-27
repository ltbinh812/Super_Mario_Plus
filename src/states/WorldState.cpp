#include "WorldState.h"
#include <raylib.h>

WorldState::WorldState() : cameraX(0.0f) {
  map.LoadMap("assets/maps/level_draft.json", "assets/maps/CombinedTiles.png",
              "assets/maps/Background.png");

  bgSky = LoadTexture("D:/Git/.assets/Legacy-Fantasy - High Forest 2.0/Legacy-Fantasy - High Forest 2.3/Background/Background.png");
  bgTrees = LoadTexture("D:/Git/.assets/Legacy-Fantasy - High Forest 2.0/Legacy-Fantasy - High Forest 2.3/Trees/Green-Tree.png");
}

WorldState::~WorldState() {
  UnloadTexture(bgSky);
  UnloadTexture(bgTrees);
}

void WorldState::HandleInput() {
  // Basic input handling
  if (IsKeyDown(KEY_RIGHT)) cameraX += 200.0f * GetFrameTime();
  if (IsKeyDown(KEY_LEFT)) cameraX -= 200.0f * GetFrameTime();
}

void WorldState::Process() {
  // Processing logic
}

void WorldState::Update(float dt) {
  // Update logic
}

void WorldState::Render(float alpha) const {
  ClearBackground(SKYBLUE);

  // Background Parallax
  float parallaxSky = cameraX * 0.1f;

  // Draw Sky (scale 3.0 to fill 720p screen)
  for (int i = -1; i < 5; i++) {
    DrawTextureEx(bgSky, {(i * bgSky.width * 3.0f) - (float)((int)parallaxSky % (bgSky.width * 3)), 0}, 0.0f, 3.0f, WHITE);
  }

  // Camera settings: Zoom 3.0x to make 16x16 tiles look like retro game
  Camera2D camera = { 0 };
  camera.target = { cameraX, 50.0f };
  camera.offset = { 0.0f, 0.0f };
  camera.rotation = 0.0f;
  camera.zoom = 3.0f;

  BeginMode2D(camera);
  map.Draw();
  EndMode2D();

  // Draw debug text
  DrawText("WORLD DRAFT: Use LEFT/RIGHT to scroll", 10, 10, 20, BLACK);
  DrawText("Diverse Background + Parallax Active", 10, 35, 20, DARKGRAY);
  DrawText("--- SMART MAP GENERATOR V2 ACTIVE ---", 200, 100, 30, RED);
}
