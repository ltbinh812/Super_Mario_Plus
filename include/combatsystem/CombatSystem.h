#pragma once
#include "ICollisionDetector.h"
#include <memory>
#include <vector>

class Player;

class CombatSystem {
    std::vector<Player*> players;
    std::unique_ptr<ICollisionDetector> detector;

public:
    CombatSystem();
    void registerPlayer(Player* p);
    void update(float dt);
    void renderDebug() const;
};
