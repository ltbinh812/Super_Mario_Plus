#pragma once
#include "GameState.h"
#include "OverlayUI.h"
#include <memory>

class World1_1State : public GameState {
public:
    World1_1State();
    ~World1_1State() override = default;

    void HandleInput() override;
    void Update(float dt) override;
    void Render(float alpha) const override;

private:
    std::unique_ptr<OverlayUI> activeOverlay_;
};
