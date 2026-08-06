#pragma once
#include "raylib.h"

class Entity;

struct Hitbox {
    Rectangle rect;         // world-space AABB
    int damage = 0;         // attack power (0 for block hitboxes)
    int defense = 0;        // damage reduction (0 for attack hitboxes)
    Entity* owner = nullptr;
    Entity* ignoreEntity = nullptr; // Ignore this entity (e.g., the spawner)
};
