#include "Punch1Skill.h"
#include "Player.h"

void Punch1Skill::execute(Player& player) {
    player.dash(100);
}
