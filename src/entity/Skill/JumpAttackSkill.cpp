#include "JumpAttackSkill.h"
#include "Player.h"

void JumpAttackSkill::execute(Player& player) {
    player.speedUpX(150);
    player.speedUpY(150);

}
