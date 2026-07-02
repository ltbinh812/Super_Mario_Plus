#pragma once 
#include "GameState.h"
#include "UIComponent.h"
#include <memory>
#include <queue>

class StateManager;

class IntroState : public GameState {
    private: 
    std::vector<std::unique_ptr<Button>> buttons;
    
    public:
    IntroState();
    void HandleInput() override;
    void Process() override;
    void Update(float dt) override; 
    void Render(float alpha) const override;

};