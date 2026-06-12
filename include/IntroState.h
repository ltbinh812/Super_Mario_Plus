#pragma once 
#include "GameState.h"
#include "UIComponent.h"
#include <memory>
#include <queue>

class IntroState : public GameState {
    private: 
    std::vector<std::unique_ptr<Button>> buttons;
    std::queue<Command> commandQueue;
    public:
    IntroState();
    void handleInput() override;
    void update(float dt) override;
    Command processCommand() override;
    void render(float alpha) const override;

};