#ifndef SETTINGSOVERLAY_H
#define SETTINGSOVERLAY_H

#include "OverlayUI.h"
#include "GameManager.h"
#include <raylib.h>

class SettingsOverlay : public OverlayUI {
public:
    SettingsOverlay(GameManager* gameManager);
    
    void HandleInput() override;
    void Update(float dt) override;
    void Draw() override;
    bool IsFinished() const override { return isFinished_; }

private:
    GameManager* gameManager_;
    bool isFinished_ = false;
};

#endif
