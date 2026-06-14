#include "SettingState.h"
#include "StateManager.h"
#include "IntroState.h"

SettingState::SettingState(){
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(); 
    menuButton->setLabel("Intro");
    menuButton->setOnClick([this](){
        PushCommand({CommandType::Change, std::make_unique<IntroState>()});
    });
    buttons.push_back(std::move(menuButton));
}

void SettingState::HandleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    for (const auto &button : buttons) {
        button->handleInput(mousePos, mousePressed, mouseReleased);
    }
}

void SettingState::Update(float dt) {
    for (const auto &button: buttons) {
        button->update();
    }
}


void SettingState::Render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}