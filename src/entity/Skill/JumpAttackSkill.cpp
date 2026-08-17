#include "JumpAttackSkill.h"
#include "Player.h"

void JumpAttackSkill::execute(Player& player) {
    player.speedUpX(50);
    player.speedUpY(150);

}
