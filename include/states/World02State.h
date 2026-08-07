#pragma once
#include "GameState.h"
#include "InputHandler.h"
#include "MapCamera.h"
#include "Player.h"
#include "TileMap.h"
#include "CombatSystem.h"
#include "CommandQueue.h"
#include <memory>
#include <vector>

class World02State : public GameState {
private:
  TileMap map;
  std::unique_ptr<Player> player1;
  std::unique_ptr<Player> player2;
  InputHandler player1Handler;
  InputHandler player2Handler;
  MapCamera mapCamera;
  CombatSystem combatSystem;

  CommandQueue spawnQueue;
  std::vector<std::unique_ptr<Entity>> activeEntities;
  std::string currentLevel;
  
public:
  World02State();
  ~World02State() override;

  void TransitionToLevel(const std::string& nextLevel, const std::string& dir, float globalX, float globalY);

  void HandleInput() override;
  void Process() override;
  void Update(float dt) override;
  void Render(float alpha) const override;
};
