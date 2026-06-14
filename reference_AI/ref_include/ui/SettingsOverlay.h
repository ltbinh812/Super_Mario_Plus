#ifndef SETTINGSOVERLAY_H
#define SETTINGSOVERLAY_H

#include "OverlayUI.h"
#include <functional>
#include "Command.h"

class SettingsOverlay : public OverlayUI {
public:
    SettingsOverlay(std::function<void(Command&&)> pushCmd);
    
    void HandleInput() override;
    void Update(float dt) override;
    void Render(float alpha) const override;
    bool IsFinished() const override { return isFinished_; }

private:
    std::function<void(Command&&)> pushCmd_;
    bool isFinished_ = false;
};

#endif
