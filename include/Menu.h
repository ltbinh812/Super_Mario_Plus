#ifndef MENU_H
#define MENU_H

#include "GameState.h"

class GameManager; // Forward declaration

class MenuState : public GameState {
public:
    MenuState(GameManager* gameManager);
    ~MenuState() override = default;

    void HandleInput() override;
    void Update(float dt) override;
    void Draw() override;

private:
    GameManager* gameManager_;
};

#endif
