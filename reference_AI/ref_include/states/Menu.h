#ifndef MENU_H
#define MENU_H

#include "GameState.h"

class MenuState : public GameState {
public:
    MenuState();
    ~MenuState() override = default;

    void HandleInput() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

private:
};

#endif
