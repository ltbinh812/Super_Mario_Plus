#include "PlayerSwimState.h"
#include "Player.h"
#include "infrastructure/AssetManager.h"
#include <cmath>

PlayerSwimState::PlayerSwimState(Player& player) : PlayerState(player) {}

void PlayerSwimState::onEnter() {
    player.playAnimation("idle"); // Use idle anim until dedicated swim anim is added
}

void PlayerSwimState::onExit() {
    StopSound(AssetManager::getInstance().getSound("swim_sound"));
}

void PlayerSwimState::update(float dt) {
    // Apply water drag on horizontal axis each frame
    player.getRuntimeStatsMutable().velocity.x *= 0.80f;
    // Only apply strong vertical drag if moving up or already slow, so we don't kill the plunge momentum early
    if (player.getRuntimeStats().velocity.y <= 40.0f) {
        player.getRuntimeStatsMutable().velocity.y *= 0.80f;
    }

    // Play swim sound periodically if moving
    if (std::abs(player.getRuntimeStats().velocity.x) > 10.0f || std::abs(player.getRuntimeStats().velocity.y) > 10.0f) {
        swimSoundTimer -= dt;
        if (swimSoundTimer <= 0.0f) {
            PlaySound(AssetManager::getInstance().getSound("swim_sound"));
            swimSoundTimer = 0.5f; // Play every 0.5 seconds while moving
        }
    } else {
        if (swimSoundTimer > 0.0f) { // If it was playing
            StopSound(AssetManager::getInstance().getSound("swim_sound"));
        }
        swimSoundTimer = 0.0f; // Reset timer when stopped
    }
}

void PlayerSwimState::onMoveLeft() {
    player.swim(-1.0f);
}

void PlayerSwimState::onMoveRight() {
    player.swim(1.0f);
}

void PlayerSwimState::onStopLeft() {
    if (player.getRuntimeStats().velocity.x < 0)
        player.getRuntimeStatsMutable().velocity.x = 0;
}

void PlayerSwimState::onStopRight() {
    if (player.getRuntimeStats().velocity.x > 0)
        player.getRuntimeStatsMutable().velocity.x = 0;
}

void PlayerSwimState::onClimb() {
    // Swim UP
    player.swimY(-1.0f);
}

void PlayerSwimState::onCrouch() {
    // Swim DOWN
    player.swimY(1.0f);
}

void PlayerSwimState::onJump() {
    auto liquid = player.getRuntimeStats().currentLiquid;
    // Only allow jumping if the player's head is outside the liquid, OR if in poison/lava
    if (player.getRuntimeStats().isPartiallyOutsideLiquid || 
        liquid == CollisionType::Poison || liquid == CollisionType::Lava) {
        player.jump();
        changePlayerState(player.jumpState);
    }
}

void PlayerSwimState::onAttack() {
    // "Punch1" KHÔNG tồn tại trong skillList — PlayerFactory chỉ đăng ký
    // Attack1..Attack4, JumpAttack, LowAttack, LongAttack, SpecialAttack, Dash,
    // Block. useSkill() tra không thấy thì lặng lẽ return, nên đánh thường dưới
    // nước trước giờ không ra đòn nào cả.
    player.useSkill("Attack1");
}
