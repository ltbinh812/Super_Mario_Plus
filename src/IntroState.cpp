#include "IntroState.h"

IntroState::IntroState() {
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(Command{CommandType::Setting}); 
    menuButton->setLabel("Setting");


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
        button->update(commandQueue);
    }
}

Command IntroState::processCommand() {
    while (!commandQueue.empty()) {
        Command t = commandQueue.front();
        commandQueue.pop();
        if (t.type == CommandType::Setting) {
            return t;
        }
    }

    return Command{CommandType::Null};
}

void IntroState::render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}