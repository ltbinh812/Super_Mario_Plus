#pragma once 
#include "GameState.h" 
#include <vector>
#include "InputHandler.h"

class Player;

class IntroState : public GameState {
    private:
    std::vector<std::unique_ptr<Player>> entities;
    std::vector<PlayerController> controllers;
    public:
    IntroState();
    ~IntroState() override; 
    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

};