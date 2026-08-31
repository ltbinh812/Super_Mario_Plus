#include "SpecialSkillAttack.h"
#include "Player.h"

void SpecialSkillAttack::execute(Player& player) {
    player.spawnSpecialBall();
}
