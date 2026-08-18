#pragma once
#include "Player.h"
#include "CharacterStats.h"

class PlayerHUD {
public:
    static void render(const Player* p1, const Player* p2, const PartyInventory* inventory);
};
