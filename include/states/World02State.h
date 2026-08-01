#pragma once
#include "GameState.h"
#include "InputHandler.h"
#include "MapCamera.h"
#include "Player.h"
#include "TileMap.h"
#include "CombatSystem.h"
#include <memory>

class World02State : public GameState {
private:
  TileMap map;
  std::unique_ptr<Player> player1;
  std::unique_ptr<Player> player2;
  InputHandler player1Handler;
  InputHandler player2Handler;
  MapCamera mapCamera;
  CombatSystem combatSystem;

public:
  World02State();
  ~World02State() override;

  void HandleInput() override;
  void Process() override;
  void Update(float dt) override;
  void Render(float alpha) const override;
};
