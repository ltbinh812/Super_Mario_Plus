#pragma once
#include "raylib.h"

class Player;

struct Hitbox {
    Rectangle rect;         // world-space AABB
    int damage = 0;         // attack power (0 for block hitboxes)
    int defense = 0;        // damage reduction (0 for attack hitboxes)
    Player* owner = nullptr;
};
