#include "PlayerStates.h"

static void HandleHorizontalMovement(Character* character, float dt) {
    auto config = character->GetInputConfig();
    // Assuming left/right shifts are for running globally, or we can use another key. For now, keep shift or add to config.
    bool running = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT); 
    float targetSpeed = running ? character->GetRunSpeed() : character->GetWalkSpeed();
    float friction = character->GetFriction();
    float vx = character->GetVelocityX();

    if (IsKeyDown(config.leftKey)) {
        character->SetFacingRight(false);
        if (vx > -targetSpeed) {
            vx -= targetSpeed * dt * friction;
            if (vx < -targetSpeed) vx = -targetSpeed;
        } else {
            // Already dashing left, let it naturally slow down to targetSpeed
            vx += 1500.0f * dt; 
            if (vx > -targetSpeed) vx = -targetSpeed;
        }
    } else if (IsKeyDown(config.rightKey)) {
        character->SetFacingRight(true);
        if (vx < targetSpeed) {
            vx += targetSpeed * dt * friction;
            if (vx > targetSpeed) vx = targetSpeed;
        } else {
            // Already dashing right, let it naturally slow down to targetSpeed
            vx -= 1500.0f * dt;
            if (vx < targetSpeed) vx = targetSpeed;
        }
    } else {
        // Normal friction when no keys are pressed
        if (vx > 0) {
            vx -= 1500.0f * dt;
            if (vx < 0) vx = 0;
        } else if (vx < 0) {
            vx += 1500.0f * dt;
            if (vx > 0) vx = 0;
        }
    }
    character->SetVelocityX(vx);
}

// --- PlayerIdleState ---
void PlayerIdleState::Update(Character* character, float dt) {
    HandleHorizontalMovement(character, dt);

    auto config = character->GetInputConfig();
    if (IsKeyPressed(config.jumpKey)) {
        if (character->IsOnGround()) {
            character->ChangeState(std::make_unique<PlayerJumpState>());
            return;
        }
    }
    if (!character->IsOnGround()) {
        character->ChangeState(std::make_unique<PlayerJumpState>());
        return;
    }
    if (fabsf(character->GetVelocityX()) > 10.0f) {
        character->ChangeState(std::make_unique<PlayerRunState>());
    }
}

// --- PlayerRunState ---
void PlayerRunState::Update(Character* character, float dt) {
    HandleHorizontalMovement(character, dt);

    auto config = character->GetInputConfig();
    if (IsKeyPressed(config.jumpKey)) {
        if (character->IsOnGround()) {
            character->ChangeState(std::make_unique<PlayerJumpState>());
            return;
        }
    }
    if (!character->IsOnGround()) {
        character->ChangeState(std::make_unique<PlayerJumpState>());
        return;
    }
    if (fabsf(character->GetVelocityX()) <= 10.0f) {
        character->ChangeState(std::make_unique<PlayerIdleState>());
    }
}

// --- PlayerJumpState ---
void PlayerJumpState::Enter(Character* character) {
    if (character->IsOnGround()) {
        character->SetVelocityY(character->GetJumpForce());
        character->SetOnGround(false);
    }
}

void PlayerJumpState::Update(Character* character, float dt) {
    HandleHorizontalMovement(character, dt);

    auto config = character->GetInputConfig();
    bool jumpHeld = IsKeyDown(config.jumpKey);
    if (!jumpHeld && character->GetVelocityY() < -200.0f) {
        // Variable jump height: apply extra gravity if jump is released early
        character->SetVelocityY(character->GetVelocityY() + character->GetGravity() * dt * 0.4f);
    }

    if (character->IsOnGround()) {
        if (fabsf(character->GetVelocityX()) > 10.0f) {
            character->ChangeState(std::make_unique<PlayerRunState>());
        } else {
            character->ChangeState(std::make_unique<PlayerIdleState>());
        }
    }
}
