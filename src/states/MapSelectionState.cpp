#include "MapSelectionState.h"
#include <iostream>

MapSelectionState::MapSelectionState() {
    backgroundTex = LoadTexture("assets/UI_screens/map_selection.png");
    
    // Fallback if image doesn't exist
    if (backgroundTex.id == 0) {
        std::cerr << "WARNING: Could not load assets/UI_screens/map_selection.png" << std::endl;
    }

    transitionIn = std::make_unique<IrisTransition>();
    transitionIn->Start(false); // false means Iris In (expanding)
    isTransitioningIn = true;
}

MapSelectionState::~MapSelectionState() {
    UnloadTexture(backgroundTex);
}

void MapSelectionState::HandleInput() {
    // Prevent input during transition
    if (isTransitioningIn) return;

    // TODO: Handle input for map selection (e.g., arrows, enter)
}

void MapSelectionState::Process() {
    if (isTransitioningIn) return;
    
    // TODO: Process logic
}

void MapSelectionState::Update(float dt) {
    if (isTransitioningIn) {
        transitionIn->Update(dt);
        if (transitionIn->IsFinished()) {
            isTransitioningIn = false;
        }
    }

    // TODO: Update logic for map selection (animations, etc.)
}

void MapSelectionState::Render(float alpha) const {
    ClearBackground(BLACK);

    if (backgroundTex.id != 0) {
        // Draw background fitted to screen
        Rectangle src = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
        Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(backgroundTex, src, dest, {0, 0}, 0.0f, WHITE);
    } else {
        // Placeholder background
        DrawText("MAP SELECTION (Missing Image)", GetScreenWidth() / 2 - 200, GetScreenHeight() / 2, 30, WHITE);
    }

    if (isTransitioningIn) {
        transitionIn->Render();
    }
}
