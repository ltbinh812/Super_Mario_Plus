#pragma once
#include "GameState.h"
#include <vector>
#include <string>

class CharacterSelectionState : public GameState {
private:
    std::vector<std::string> characters = {
        "Goku", "Naruto", "Luffy", "Kakashi", "Sasuke", "Zoro"
    };
    int selectedIndex = 0;

public:
    CharacterSelectionState();
    ~CharacterSelectionState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
