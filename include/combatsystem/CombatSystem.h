#pragma once
#include "ICollisionDetector.h"
#include <memory>
#include <vector>

class Entity;

class CombatSystem {
    std::vector<Entity*> entities;
    std::unique_ptr<ICollisionDetector> detector;

public:
    CombatSystem();
    void registerEntity(Entity* e);
    void removeInactive();
    void update(float dt);
    void renderDebug() const;
};
