#pragma once 
#include "GameState.h"
#include "UIComponent.h"
#include <memory>
#include <queue>

class StateManager;

class SettingState : public GameState {
    private: 
    std::vector<std::unique_ptr<Button>> buttons;
    public:
    SettingState();
    void HandleInput() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

};