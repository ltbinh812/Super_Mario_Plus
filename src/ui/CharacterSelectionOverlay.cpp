#include "CharacterSelectionOverlay.h"
#include "CharacterFactory.h"

CharacterSelectionOverlay::CharacterSelectionOverlay(std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2, const std::vector<std::string>& availableChars)
    : player1Ref_(p1), player2Ref_(p2), availableChars_(availableChars) 
{
    // Try to find the current player index if needed, for simplicity temporarily start from 0 and 1 or from variable
}

void CharacterSelectionOverlay::HandleInput() {
    if (IsKeyPressed(KEY_U)) {
        isFinished_ = true;
        return;
    }

    // Player 1 change
    if (IsKeyPressed(KEY_A)) {
        p1Index_ = (p1Index_ - 1 + availableChars_.size()) % availableChars_.size();
        auto oldPos = player1Ref_->GetPosition();
        auto oldConfig = player1Ref_->GetInputConfig();
        player1Ref_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableChars_[p1Index_]).release()));
        player1Ref_->Init(oldPos.x, oldPos.y);
        player1Ref_->SetInputConfig(oldConfig);
    }
    if (IsKeyPressed(KEY_D)) {
        p1Index_ = (p1Index_ + 1) % availableChars_.size();
        auto oldPos = player1Ref_->GetPosition();
        auto oldConfig = player1Ref_->GetInputConfig();
        player1Ref_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableChars_[p1Index_]).release()));
        player1Ref_->Init(oldPos.x, oldPos.y);
        player1Ref_->SetInputConfig(oldConfig);
    }

    // Player 2 change
    if (IsKeyPressed(KEY_LEFT)) {
        p2Index_ = (p2Index_ - 1 + availableChars_.size()) % availableChars_.size();
        auto oldPos = player2Ref_->GetPosition();
        auto oldConfig = player2Ref_->GetInputConfig();
        player2Ref_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableChars_[p2Index_]).release()));
        player2Ref_->Init(oldPos.x, oldPos.y);
        player2Ref_->SetInputConfig(oldConfig);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        p2Index_ = (p2Index_ + 1) % availableChars_.size();
        auto oldPos = player2Ref_->GetPosition();
        auto oldConfig = player2Ref_->GetInputConfig();
        player2Ref_ = std::unique_ptr<Player>(static_cast<Player*>(CharacterFactory::GetInstance().CreateCharacter(availableChars_[p2Index_]).release()));
        player2Ref_->Init(oldPos.x, oldPos.y);
        player2Ref_->SetInputConfig(oldConfig);
    }
}

void CharacterSelectionOverlay::Update(float dt) {
}

void CharacterSelectionOverlay::Draw() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    DrawText("CHARACTER SELECTION (Press U to resume)", GetScreenWidth()/2 - 250, 100, 20, WHITE);
    
    DrawText("Player 1 (A/D to swap):", 100, 200, 20, RED);
    DrawText(availableChars_[p1Index_].c_str(), 100, 240, 30, WHITE);

    DrawText("Player 2 (Left/Right to swap):", 500, 200, 20, BLUE);
    DrawText(availableChars_[p2Index_].c_str(), 500, 240, 30, WHITE);
}
