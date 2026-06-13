#include "SettingState.h"
#include "StateManager.h"
#include "IntroState.h"

SettingState::SettingState(){
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(); 
    menuButton->setLabel("Intro");
    menuButton->setOnClick([this](){
        pushCommand({CommandType::Change, std::make_unique<IntroState>()});
    });
    buttons.push_back(std::move(menuButton));
}

void SettingState::handleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    for (const auto &button : buttons) {
        button->handleInput(mousePos, mousePressed, mouseReleased);
    }
}

void SettingState::update(float dt) {
    for (const auto &button: buttons) {
        button->update();
    }
}


void SettingState::render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}