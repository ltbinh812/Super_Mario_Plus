#ifndef CHARACTERSELECTIONOVERLAY_H
#define CHARACTERSELECTIONOVERLAY_H

#include "OverlayUI.h"
#include "Player.h"
#include <vector>
#include <string>
#include <memory>
#include <raylib.h>

class CharacterSelectionOverlay : public OverlayUI {
public:
    CharacterSelectionOverlay(std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2, const std::vector<std::string>& availableChars);
    
    void HandleInput() override;
    void Update(float dt) override;
    void Draw() override;
    bool IsFinished() const override { return isFinished_; }

private:
    std::unique_ptr<Player>& player1Ref_;
    std::unique_ptr<Player>& player2Ref_;
    std::vector<std::string> availableChars_;
    
    int p1Index_ = 0;
    int p2Index_ = 1;
    bool isFinished_ = false;
};

#endif
