#pragma once
#include "GameState.h"
#include "IrisTransition.h"
#include <raylib.h>
#include <memory>

class MapSelectionState : public GameState {
private:
    Texture2D backgroundTex;
    std::unique_ptr<ITransition> transitionIn;
    bool isTransitioningIn;

public:
    MapSelectionState();
    ~MapSelectionState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
