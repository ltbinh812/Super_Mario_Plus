#include "LoadingState.h"
#include "StateCommands.h"
#include <iostream>

LoadingState::LoadingState(std::function<std::unique_ptr<GameState>()> factory, float duration)
    : stateFactory(std::move(factory)), targetState(nullptr), timer(0.0f), maxTime(duration), loadingText("LOADING"), dotCount(0), dotTimer(0.0f), hasRendered(false) {
    
    // Randomize the pause threshold between 40% and 80% (0.4f to 0.8f)
    pauseThreshold = GetRandomValue(40, 80) / 100.0f;
    
    // Attempt to load custom font, fallback to default if fails
    customFont = LoadFont("assets/config/kenney-pixel-hu.otf");
}

LoadingState::~LoadingState() {
    if (customFont.texture.id != 0) {
        UnloadFont(customFont);
    }
}

void LoadingState::HandleInput() {
    // Ignore input during loading
}

void LoadingState::Process() {
    // Nothing to process
}

void LoadingState::Update(float dt) {
    if (!hasRendered) return; // Skip catch-up loop updates before the first visual frame
    
    // If we haven't loaded the target state yet, and we reached the random pause threshold
    if (!targetState && timer >= maxTime * pauseThreshold) {
        // This will block the main thread while heavy assets load!
        targetState = stateFactory(); 
        
        // After loading finishes, force the timer to 100% to jump the bar to the end
        timer = maxTime;
    }
    
    if (targetState) {
        // If loaded, just wait for timer to reach maxTime (which it should be already)
        if (timer < maxTime) timer += dt;
        if (timer >= maxTime) {
            this->PushStateCommand(std::make_unique<::ChangeStateCommand>(std::move(targetState)));
        }
    } else {
        timer += dt;
        dotTimer += dt;
        
        if (dotTimer > 0.3f) {
            dotTimer = 0.0f;
            dotCount = (dotCount + 1) % 4; // 0, 1, 2, 3
        }
    }
}

void LoadingState::Render(float alpha) const {
    hasRendered = true; // Mark as rendered so Update can start progressing logic
    
    ClearBackground(BLACK);
    
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    
    // 1. Draw Progress Bar
    float barWidth = 400.0f;
    float barHeight = 20.0f;
    float barX = (screenW - barWidth) / 2.0f;
    float barY = (screenH - barHeight) / 2.0f + 50.0f; // Slightly below center
    
    // Progress calculation
    float progress = timer / maxTime;
    if (progress > 1.0f) progress = 1.0f;
    
    // Outline
    DrawRectangleLinesEx({barX - 2, barY - 2, barWidth + 4, barHeight + 4}, 2.0f, DARKGRAY);
    // Background
    DrawRectangleRec({barX, barY, barWidth, barHeight}, {30, 30, 30, 255});
    // Fill
    DrawRectangleRec({barX, barY, barWidth * progress, barHeight}, {255, 203, 0, 255}); // Gold/Yellow fill
    
    // 2. Draw Text
    std::string displayTxt = loadingText;
    for (int i = 0; i < dotCount; ++i) {
        displayTxt += ".";
    }
    
    float fontSize = 40.0f;
    Vector2 textSize = {0, 0};
    
    if (customFont.texture.id != 0) {
        textSize = MeasureTextEx(customFont, "LOADING...", fontSize, 1.0f); // Measure max length
        Vector2 textPos = { (screenW - textSize.x) / 2.0f, barY - textSize.y - 20.0f };
        DrawTextEx(customFont, displayTxt.c_str(), textPos, fontSize, 1.0f, WHITE);
    } else {
        textSize.x = (float)MeasureText("LOADING...", (int)fontSize);
        textSize.y = fontSize;
        Vector2 textPos = { (screenW - textSize.x) / 2.0f, barY - textSize.y - 20.0f };
        DrawText(displayTxt.c_str(), (int)textPos.x, (int)textPos.y, (int)fontSize, WHITE);
    }
}
