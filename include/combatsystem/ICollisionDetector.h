#pragma once
#include "Hitbox.h"
#include <vector>

class Player;

struct CollisionPair {
    const Hitbox* hitbox;
    Player* target;
};

class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    virtual std::vector<CollisionPair> detect(
        const std::vector<Hitbox>& hitboxes,
        const std::vector<Player*>& entities
    ) = 0;
};
