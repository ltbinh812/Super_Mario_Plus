#include "GameManager.h"

GameManager::GameManager() : currentState_(nullptr) {}

void GameManager::ChangeState(std::unique_ptr<GameState> newState) {
    currentState_ = std::move(newState);
}

void GameManager::HandleInput() {
    if (currentState_) {
        currentState_->HandleInput();
    }
}

void GameManager::Update(float dt) {
    if (currentState_) {
        currentState_->Update(dt);
    }
}

void GameManager::Draw() {
    if (currentState_) {
        currentState_->Draw();
    }
}
