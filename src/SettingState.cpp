#include "SettingState.h"

SettingState::SettingState() {
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(Command{CommandType::Intro}); 
    menuButton->setLabel("Intro");

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
        button->update(commandQueue);
    }
}

Command SettingState::processCommand() {
    while (!commandQueue.empty()) {
        Command t = commandQueue.front();
        commandQueue.pop();
        if (t.type == CommandType::Intro) {
            return t;
        }
    }

    return Command{CommandType::Null};
}

void SettingState::render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}