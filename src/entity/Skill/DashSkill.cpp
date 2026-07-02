#include "DashSkill.h"
#include "Player.h"

DashSkill::DashSkill() {
  skillName = "dash";
  cooldownTime = 1.0f;
  currentCooldown = 0.0f;
  manaCost = 10.0f;
}

void DashSkill::execute(Player &player) {
  float dashSpeed = player.getFaceDirection() ? 1200.0f : -1200.0f;
  player.setVelocityX(dashSpeed);
}