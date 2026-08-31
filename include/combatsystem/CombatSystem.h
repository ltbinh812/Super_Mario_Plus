#pragma once
#include "ICollisionDetector.h"
#include <memory>
#include <vector>

class Entity;

class CombatSystem {
    std::unique_ptr<ICollisionDetector> detector;

public:
    CombatSystem();
    void update(const std::vector<Entity*>& entities, float dt);
    void renderDebug(const std::vector<Entity*>& entities) const;
};
