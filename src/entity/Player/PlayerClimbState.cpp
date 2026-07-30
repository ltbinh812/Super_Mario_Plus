#include "PlayerClimbState.h"
#include "Player.h"
#include "PlayerStates.h"

PlayerClimbState::PlayerClimbState(Player &player) : PlayerState(player) {}

void PlayerClimbState::onEnter() {
  player.playAnimation("Idle"); // Tạm thời
  player.getRuntimeStatsMutable().velocity = {0.0f, 0.0f};
  // Vô hiệu hóa trọng lực bằng cách set mask (hoặc Entity xử lý onOverlapLadder
  // bỏ qua trọng lực) Tạm thời, set velocity.y = 0 mỗi frame
}

void PlayerClimbState::onExit() {}

void PlayerClimbState::update(float dt) {
  if (!player.getRuntimeStats().isOverlappingLadder) {
    player.requestState(player.fallState);
    return;
  }
  // Nếu không bấm phím leo (lên/xuống), nhân vật sẽ tự động tuột xuống từ từ
  player.getRuntimeStatsMutable().velocity.y =
      player.getBaseStats().moveVelocity * 0.000001f;
  player.getRuntimeStatsMutable().velocity.x = 0.0f;
}

void PlayerClimbState::onMoveLeft() {
  player.getRuntimeStatsMutable().velocity.x =
      -player.getBaseStats().moveVelocity;
}

void PlayerClimbState::onMoveRight() {
  player.getRuntimeStatsMutable().velocity.x =
      player.getBaseStats().moveVelocity;
}

void PlayerClimbState::onJump() {
  // Nhảy khỏi thang
  player.getRuntimeStatsMutable().ignoreLadderTimer = 0.3f;
  player.requestState(player.jumpState);
  player.jump();
}

void PlayerClimbState::onClimb() {
  // Leo lên
  player.getRuntimeStatsMutable().velocity.y =
      -player.getBaseStats().moveVelocity * 0.5f;
}

void PlayerClimbState::onCrouch() {
  // Leo xuống nhanh hơn khi giữ phím xuống
  player.getRuntimeStatsMutable().velocity.y =
      player.getBaseStats().moveVelocity * 0.5f;
}
