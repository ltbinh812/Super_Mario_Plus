#include "entity/Player/PlayerFactory.h"
#include <iostream>

int main() {
    auto player = PlayerFactory::createPlayer("Goku", {0, 0});
    if (player) {
        std::cout << "Player created successfully." << std::endl;
        std::cout << "Skill Dash exists: " << (player->findSkill("Dash") != nullptr) << std::endl;
    } else {
        std::cout << "Failed to create player." << std::endl;
    }
    return 0;
}
