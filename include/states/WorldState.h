#pragma once
#include "GameState.h"
#include "TileMap.h"

class WorldState : public GameState {
private:
  TileMap map;
  
  // Extra textures for diverse background
  Texture2D bgSky;
  Texture2D bgTrees;

  float cameraX;

public:
  WorldState();
  ~WorldState() override;

  void HandleInput() override;
  void Process() override;
  void Update(float dt) override;
  void Render(float alpha) const override;
};
