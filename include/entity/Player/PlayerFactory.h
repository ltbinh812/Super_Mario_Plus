#pragma once
#include "Player.h"
#include <memory>
#include <string>

class PlayerFactory {
public:
    static std::unique_ptr<Player> createPlayer(const std::string& charName, Vector2 pos);
};
