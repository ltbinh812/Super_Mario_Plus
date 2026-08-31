#pragma once
#include <raylib.h>
#include <string>
#include <functional>

class Entity;

enum class SpawnCategory {
    Entity,
    Item
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
    std::function<void(Entity*)> onHitEffect = nullptr;
};
