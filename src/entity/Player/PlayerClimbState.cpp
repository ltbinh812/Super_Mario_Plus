#include "PlayerClimbState.h"
#include "Player.h"

PlayerClimbState::PlayerClimbState(Player &player) : PlayerState(player) {}

void PlayerClimbState::onEnter() {
    player.playAnimation("idle"); // Placeholder — replace with "climb" anim when available
    player.stopClimb();           // Freeze on ladder on entry
}

void PlayerClimbState::onExit() {}

void PlayerClimbState::update(float dt) {
    // Exit climb state if no longer on a ladder
    if (!player.getRuntimeStats().isOverlappingLadder) {
        player.requestState(player.fallState);
        return;
    }
    // Hold in place if no climb input — cancel any residual gravity
    player.stopClimb();
}

void PlayerClimbState::onMoveLeft() {
    player.moveLeft();
}

void PlayerClimbState::onMoveRight() {
    player.moveRight();
}

void PlayerClimbState::onJump() {
    // Jump off ladder: set ignore timer so player doesn't immediately re-enter ClimbState
    player.getRuntimeStatsMutable().ignoreLadderTimer = 0.3f;
    player.jump();
    player.requestState(player.jumpState);
}

void PlayerClimbState::onClimb() {
    // KEY_UP or W: climb up the ladder
    player.climbUp();
}

void PlayerClimbState::onCrouch() {
    // KEY_DOWN or S: climb down the ladder
    player.climbDown();
}
