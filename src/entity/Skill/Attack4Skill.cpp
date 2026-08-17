#include "Attack4Skill.h"
#include "Player.h"

void Attack4Skill::execute(Player& player) {
    player.spawnExplosion();
}
