#pragma once
#include "raylib.h"
#include <functional>

class Entity;

#include "EntityFaction.h"
#include <cstdint>

struct Hitbox {
    Rectangle rect;         // world-space AABB
    int damage = 0;         // attack power (0 for block hitboxes)
    int defense = 0;        // damage reduction (0 for attack hitboxes)
    Entity* owner = nullptr;
    Entity* ignoreEntity = nullptr; // Ignore this entity (e.g., the spawner)
    std::function<void(Entity*)> onHitEffect = nullptr;
    uint32_t targetFactionMask = 0xFFFFFFFF; // By default hit all factions

    Hitbox(Rectangle r = {0,0,0,0}, int dmg = 0, int def = 0, Entity* o = nullptr, Entity* i = nullptr, std::function<void(Entity*)> eff = nullptr, uint32_t mask = 0xFFFFFFFF)
        : rect(r), damage(dmg), defense(def), owner(o), ignoreEntity(i), onHitEffect(eff), targetFactionMask(mask) {}

    bool canHit(EntityFaction f) const {
        if (f == EntityFaction::None) return true; // Or decide otherwise
        return (targetFactionMask & (1 << static_cast<int>(f))) != 0;
    }
};
