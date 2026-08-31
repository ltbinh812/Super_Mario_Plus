#include "BlockSkill.h"
#include "Player.h"

void BlockSkill::execute(Player& player) {
    // Block = stand guard, stop all movement
    player.idle();
}
