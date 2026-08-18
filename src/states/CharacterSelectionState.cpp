#include "CharacterSelectionState.h"
#include "World02State.h"
#include "StateCommands.h"
#include "raylib.h"
#include <iostream>

CharacterSelectionState::CharacterSelectionState() {
    std::cout << "[CharacterSelectionState] Entered character selection menu.\n";
}

CharacterSelectionState::~CharacterSelectionState() = default;

void CharacterSelectionState::HandleInput() {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex++;
        if (selectedIndex >= characters.size()) {
            selectedIndex = 0;
        }
    }
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex--;
        if (selectedIndex < 0) {
            selectedIndex = characters.size() - 1;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_J)) {
        std::string selectedChar = characters[selectedIndex];
        std::cout << "[CharacterSelectionState] Selected character: " << selectedChar << "\n";
        PushStateCommand(std::make_unique<ChangeStateCommand>(std::make_unique<World02State>(selectedChar)));
    }
}

void CharacterSelectionState::Process() {
}

void CharacterSelectionState::Update(float dt) {
}

void CharacterSelectionState::Render(float alpha) const {
    ClearBackground(DARKBLUE);

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const char* title = "SELECT YOUR CHARACTER";
    int titleFontSize = 40;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, screenWidth / 2 - titleWidth / 2, 50, titleFontSize, YELLOW);

    int startY = 150;
    int spacing = 50;

    for (int i = 0; i < characters.size(); i++) {
        Color color = (i == selectedIndex) ? GREEN : WHITE;
        std::string text = characters[i];
        if (i == selectedIndex) {
            text = "> " + text + " <";
        }

        int fontSize = (i == selectedIndex) ? 30 : 25;
        int textWidth = MeasureText(text.c_str(), fontSize);
        
        DrawText(text.c_str(), screenWidth / 2 - textWidth / 2, startY + i * spacing, fontSize, color);
    }

    const char* instructions = "Use UP/DOWN to navigate. Press ENTER to select.";
    int instWidth = MeasureText(instructions, 20);
    DrawText(instructions, screenWidth / 2 - instWidth / 2, screenHeight - 50, 20, LIGHTGRAY);
}
