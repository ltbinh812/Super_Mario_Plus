#include "DashSkill.h"
#include "Player.h"

void DashSkill::execute(Player& player) {
    // Apply a velocity burst in the facing direction
    float dashSpeed = 600.0f;
    player.dash(dashSpeed);
}
