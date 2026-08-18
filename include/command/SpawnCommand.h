#pragma once
#include <raylib.h>
#include <string>

class Entity;

enum class EntityType {
    Fireball,
    Explosion,
    SpecialBall
};

struct SpawnCommand {
    EntityType type;
    Vector2 position;
    bool isFacingRight;
    std::string ownerName;  // character name for EntityFactory JSON config lookup
    Entity* spawner = nullptr; // the entity that spawned this
};
