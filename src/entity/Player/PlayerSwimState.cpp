#include "PlayerSwimState.h"
#include "Player.h"
#include "PlayerStates.h"

PlayerSwimState::PlayerSwimState(Player& player) : PlayerState(player) {}

void PlayerSwimState::onEnter() {
    // Tạm thời dùng hoạt ảnh Run hoặc Idle, bạn có thể thay bằng "Swim" sau này
    player.playAnimation("Run");
    player.getRuntimeStatsMutable().velocity.y = 0.0f;
    // Lưu trọng lực gốc để khôi phục
    // Giảm trọng lực để lơ lửng trong nước
    // Thực tế trọng lực có thể được kiểm soát ở Entity, nhưng ta cũng có thể set trực tiếp
}

void PlayerSwimState::onExit() {
}

void PlayerSwimState::update(float dt) {
    // Lực nổi nhẹ hoặc rơi chậm
    CharacterRuntimeStats& rStats = player.getRuntimeStatsMutable();
    
    // Áp dụng thêm ma sát nước vào trục X
    rStats.velocity.x *= 0.95f; 
    
    // Nếu chạm đáy hoặc bơi ra khỏi nước, Player tự xử lý dựa vào hook
}

void PlayerSwimState::onMoveLeft() {
    player.getWorldStatsMutable().isFacingRight = false;
    player.getRuntimeStatsMutable().velocity.x = -player.getBaseStats().moveVelocity * 0.7f; // Bơi chậm hơn
}

void PlayerSwimState::onMoveRight() {
    player.getWorldStatsMutable().isFacingRight = true;
    player.getRuntimeStatsMutable().velocity.x = player.getBaseStats().moveVelocity * 0.7f;
}

void PlayerSwimState::onStopLeft() {
    if (player.getRuntimeStats().velocity.x < 0) {
        player.getRuntimeStatsMutable().velocity.x = 0;
    }
}

void PlayerSwimState::onStopRight() {
    if (player.getRuntimeStats().velocity.x > 0) {
        player.getRuntimeStatsMutable().velocity.x = 0;
    }
}

void PlayerSwimState::onJump() {
    // Ngoi lên
    player.getRuntimeStatsMutable().velocity.y = -player.getBaseStats().jumpVelocity * 0.6f;
}
