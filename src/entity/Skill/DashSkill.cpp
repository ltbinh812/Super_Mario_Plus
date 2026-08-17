#include "DashSkill.h"
#include "Player.h"

void DashSkill::execute(Player& player) {
    // Use smaller hitbox while dashing (must be called before dash to not overwrite velocity)
    player.crouch();
    // Apply a velocity burst in the facing direction
    float dashSpeed = 600.0f;
    player.speedUpX(dashSpeed);
}
