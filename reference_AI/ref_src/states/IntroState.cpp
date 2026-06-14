#include "IntroState.h"
#include "StateManager.h"
#include "SettingState.h"
#include "Menu.h"

IntroState::IntroState(){
    std::unique_ptr<Button> menuButton = std::make_unique<Button>(); 
    menuButton->setLabel("Setting");
    menuButton->setPosition({100, 100});
    menuButton->setOnClick([this](){
        PushCommand({CommandType::Change, std::make_unique<SettingState>()});
    });

    std::unique_ptr<Button> playButton = std::make_unique<Button>();
    playButton->setLabel("Play");
    playButton->setPosition({100, 200});
    playButton->setOnClick([this](){
        PushCommand({CommandType::Change, std::make_unique<MenuState>()});
    });

    buttons.push_back(std::move(menuButton));
    buttons.push_back(std::move(playButton));
}

void IntroState::HandleInput() {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    for (const auto &button : buttons) {
        button->handleInput(mousePos, mousePressed, mouseReleased);
    }
}

void IntroState::Update(float dt) {
    for (const auto &button: buttons) {
        button->update();
    }

}


void IntroState::Render(float alpha) const {
    for (const auto &button: buttons) {
        button->render();
    }
}