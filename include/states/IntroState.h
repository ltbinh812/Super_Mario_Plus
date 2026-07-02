#pragma once 
#include "GameState.h" 
#include <vector>
#include "InputHandler.h"

class Entity;

class IntroState : public GameState {
    private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<PlayerController> controllers;
    public:
    IntroState();
    ~IntroState() = default; 
    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

};