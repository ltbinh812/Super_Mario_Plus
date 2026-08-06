#pragma once
#include "ICollisionDetector.h"

class BruteForceDetector : public ICollisionDetector {
public:
    std::vector<CollisionPair> detect(
        const std::vector<Hitbox>& hitboxes,
        const std::vector<Entity*>& entities
    ) override;
};
