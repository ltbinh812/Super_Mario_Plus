#pragma once
#include <raylib.h>
#include <string>

class Entity;

enum class SpawnCategory {
    Entity,
    Item,
    ExplosionDamage  // Boom pushes this when exploding; Process() handles area damage
};

enum class EntityType {
    Fireball,
    Explosion,
    SpecialBall
};

struct SpawnCommand {
    SpawnCategory category = SpawnCategory::Entity;
    EntityType type; // Used if category == Entity
    std::string itemIdentifier; // Used if category == Item (e.g. "Coin", "Buff", "Boom")
    Vector2 position;
    Vector2 velocity = {0.0f, 0.0f}; // Used for thrown items
    bool isFacingRight = true;
    std::string ownerName;  // character name for EntityFactory JSON config lookup
    std::string iid;        // Instance ID from LDtk for state tracking
    Entity* spawner = nullptr; // the entity that spawned this
    // Used for ExplosionDamage category:
    Rectangle explosionRect = {0, 0, 0, 0}; // World-space blast rect (5x3 blocks)
};
