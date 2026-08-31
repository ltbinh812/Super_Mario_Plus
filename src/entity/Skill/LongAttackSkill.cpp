#include "LongAttackSkill.h"
#include "Player.h"
void LongAttackSkill::execute(Player& player) {
    player.spawnFireball();
}