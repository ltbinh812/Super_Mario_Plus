#pragma once
#include "GameState.h"
#include <raylib.h>
#include <memory>
#include <string>
#include <functional>

class LoadingState : public GameState {
private:
    std::function<std::unique_ptr<GameState>()> stateFactory;
    std::unique_ptr<GameState> targetState;
    float timer;
    float maxTime;
    std::string loadingText;
    int dotCount;
    float dotTimer;
    float pauseThreshold;
    Font customFont; // Optional, can use default font if not loaded
    mutable bool hasRendered;

public:
    // Creates a loading state that waits for `duration` seconds, loads the state via factory at 80%, then transitions.
    LoadingState(std::function<std::unique_ptr<GameState>()> factory, float duration);
    ~LoadingState() override;

    void HandleInput() override;
    void Process() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
};
