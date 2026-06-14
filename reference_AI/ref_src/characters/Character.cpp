#include "Character.h"
#include <cmath>

Character::Character() : onGround_(false), facingRight_(true) {
    currentState_ = nullptr;
    ability1_ = nullptr;
    ability2_ = nullptr;
}

Character::~Character() {}

void Character::Init(float startX, float startY) {
    pos_ = { startX, startY };
    vel_ = { 0.0f, 0.0f };
    onGround_ = false;
    facingRight_ = true;
    if (currentState_) currentState_->Enter(this);
}

void Character::ChangeState(std::unique_ptr<CharacterState> newState) {
    if (currentState_) {
        currentState_->Exit(this);
    }
    currentState_ = std::move(newState);
    if (currentState_) {
        currentState_->Enter(this);
    }
}

void Character::SetAbility1(std::unique_ptr<AbilityStrategy> newAbility) {
    ability1_ = std::move(newAbility);
}

void Character::SetAbility2(std::unique_ptr<AbilityStrategy> newAbility) {
    ability2_ = std::move(newAbility);
}

void Character::UseAbility1() {
    if (ability1_) {
        ability1_->Execute(this);
    }
}

void Character::UseAbility2() {
    if (ability2_) {
        ability2_->Execute(this);
    }
}

void Character::Update(float dt, float worldWidth) {
    if (currentState_) {
        currentState_->Update(this, dt);
    }
    ApplyPhysics(dt, worldWidth);
}

void Character::ApplyPhysics(float dt, float worldWidth) {
    vel_.y += gravity_ * dt;
    if (vel_.y > maxFall_) vel_.y = maxFall_;

    pos_.x += vel_.x * dt;
    pos_.y += vel_.y * dt;

    if (pos_.x < 0) pos_.x = 0;
    if (pos_.x + width_ > worldWidth) pos_.x = worldWidth - width_;

    if (pos_.y > 720 + 200) { // Respawn threshold, maybe customize per character later
        pos_ = { 200.0f, 720 - 80.0f - height_ };
        vel_ = { 0, 0 };
    }
}

void Character::ResolveCollision(Rectangle platRect) {
    Rectangle pb = GetBounds();

    if (!CheckCollisionRecs(pb, platRect)) return;

    float overlapLeft   = (pb.x + pb.width)  - platRect.x;
    float overlapRight  = (platRect.x + platRect.width) - pb.x;
    float overlapTop    = (pb.y + pb.height)  - platRect.y;
    float overlapBottom = (platRect.y + platRect.height) - pb.y;

    bool fromLeft   = overlapLeft   < overlapRight && overlapLeft   < overlapTop && overlapLeft   < overlapBottom;
    bool fromRight  = overlapRight  < overlapLeft  && overlapRight  < overlapTop && overlapRight  < overlapBottom;
    bool fromTop    = overlapTop    < overlapBottom && overlapTop   < overlapLeft && overlapTop   < overlapRight;
    bool fromBottom = overlapBottom < overlapTop   && overlapBottom < overlapLeft && overlapBottom < overlapRight;

    if (fromTop) {
        pos_.y  = platRect.y - pb.height;
        vel_.y  = 0.0f;
        onGround_ = true;
    } else if (fromBottom) {
        pos_.y  = platRect.y + platRect.height;
        vel_.y  = fabsf(vel_.y) * 0.1f;
    } else if (fromLeft) {
        pos_.x  = platRect.x - pb.width;
        vel_.x  = 0.0f;
    } else if (fromRight) {
        pos_.x  = platRect.x + platRect.width;
        vel_.x  = 0.0f;
    }
}

void Character::Draw() {
    // Base characters might not draw anything or draw a debug rect.
    DrawRectangle((int)pos_.x, (int)pos_.y, (int)width_, (int)height_, RED);
}
