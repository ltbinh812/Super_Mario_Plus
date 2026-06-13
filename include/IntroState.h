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
    void handleInput() override;
    void update(float dt) override; 
    void render(float alpha) const override;

};