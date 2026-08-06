#pragma once
#include "Hitbox.h"
#include <vector>

class Entity;

struct CollisionPair {
    const Hitbox* hitbox;
    Entity* target;
};

class ICollisionDetector {
public:
    virtual ~ICollisionDetector() = default;
    virtual std::vector<CollisionPair> detect(
        const std::vector<Hitbox>& hitboxes,
        const std::vector<Entity*>& entities
    ) = 0;
};
