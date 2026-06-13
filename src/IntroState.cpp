#include "IntroState.h"
#include "StateManager.h"
#include "SettingState.h"
IntroState::IntroState(){
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(); 
    menuButton->setLabel("Setting");
    menuButton->setOnClick([this](){
        pushCommand({CommandType::Change, std::make_unique<SettingState>()});
    });


    buttons.push_back(std::move(menuButton));
}

void IntroState::handleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    for (const auto &button : buttons) {
        button->handleInput(mousePos, mousePressed, mouseReleased);
    }
}

void IntroState::update(float dt) {
    for (const auto &button: buttons) {
        button->update();
    }

}


void IntroState::render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}