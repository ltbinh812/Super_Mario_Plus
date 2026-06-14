#include "CharacterSelectionOverlay.h"
#include "CharacterFactory.h"

CharacterSelectionOverlay::CharacterSelectionOverlay(std::unique_ptr<Player>& p1, std::unique_ptr<Player>& p2, const std::vector<std::string>& availableChars)
    : player1Ref_(p1), player2Ref_(p2), availableChars_(availableChars) 
{
    // Thử tìm chỉ số người chơi hiện tại nếu cần, để đơn giản tạm thời bắt đầu từ 0 và 1 hoặc từ biến
}

void CharacterSelectionOverlay::HandleInput() {
    if (IsKeyPressed(KEY_U)) {
        isFinished_ = true;
        return;
    }

    // Player 1 đổi
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

    // Player 2 đổi
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

void CharacterSelectionOverlay::Render(float alpha) const {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    DrawText("CHARACTER SELECTION", GetScreenWidth()/2 - 150, 50, 30, YELLOW);
    
    DrawText("PLAYER 1", GetScreenWidth()/4 - 50, 150, 20, RAYWHITE);
    DrawText(availableChars_[p1Index_].c_str(), GetScreenWidth()/4 - 50, 200, 30, RED);
    DrawText("Use A/D to change", GetScreenWidth()/4 - 50, 250, 15, GRAY);

    DrawText("PLAYER 2", 3*GetScreenWidth()/4 - 50, 150, 20, RAYWHITE);
    DrawText(availableChars_[p2Index_].c_str(), 3*GetScreenWidth()/4 - 50, 200, 30, BLUE);
    DrawText("Use LEFT/RIGHT to change", 3*GetScreenWidth()/4 - 50, 250, 15, GRAY);

    DrawText("Press ENTER to confirm", GetScreenWidth()/2 - 120, GetScreenHeight() - 100, 20, GREEN);
}
