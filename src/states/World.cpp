#include "World.h"
#include "Menu.h"
#include "SettingsOverlay.h"
#include "raylib.h"

World1_1State::World1_1State() {
    // SKELETON: No entities yet. Ready for your code!
}

void World1_1State::HandleInput() {
    if (activeOverlay_) {
        activeOverlay_->HandleInput();
        if (activeOverlay_->IsFinished()) {
            activeOverlay_.reset();
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        activeOverlay_ = std::make_unique<SettingsOverlay>([this](Command&& cmd) {
            this->PushCommand(std::move(cmd));
        });
        return;
    }
}

void World1_1State::Update(float dt) {
    if (activeOverlay_) {
        activeOverlay_->Update(dt);
        return;
    }

    // Update physical entities here
}

void World1_1State::Render(float alpha) const {
    ClearBackground(SKYBLUE);
    
    DrawText("WORLD SKELETON - READY TO CODE", 250, 300, 20, DARKGRAY);
    DrawText("Press ESC to open Settings", 250, 340, 20, DARKGRAY);

    if (activeOverlay_) {
        activeOverlay_->Render(alpha);
    }
}
