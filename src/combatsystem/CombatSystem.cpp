#include "CombatSystem.h"
#include "BruteForceDetector.h"
#include "Entity.h"
#include "raylib.h"
#include <algorithm>
#include <iostream>

CombatSystem::CombatSystem()
    : detector(std::make_unique<BruteForceDetector>()) {}

void CombatSystem::update(const std::vector<Entity*>& entities, float dt) {
    // 1. Collect active hitboxes from all entities
    std::vector<Hitbox> activeHitboxes;
    for (auto* entity : entities) {
        if (!entity->getIsActive()) continue;
        if (entity->hasActiveHitbox()) {
            activeHitboxes.push_back(entity->getActiveHitbox());
        }
    }

    if (activeHitboxes.empty()) return;

    // 2. Run collision detection (attack hitboxes vs entity physics boxes)
    std::vector<CollisionPair> collisions = detector->detect(activeHitboxes, entities);

    // 3. Resolve damage for each collision
    for (const auto& pair : collisions) {
        const Hitbox* attackBox = pair.hitbox;
        Entity* target = pair.target;

        // Skip block-type hitboxes (defense only, no outgoing damage)
        if (attackBox->damage <= 0) continue;

        // Check if target is actively blocking
        int targetDefense = 0;
        if (target->hasActiveHitbox()) {
            Hitbox targetHB = target->getActiveHitbox();
            if (targetHB.defense > 0) {
                targetDefense = targetHB.defense;
            }
        }

        int finalDamage = std::max(0, attackBox->damage - targetDefense);
        if (finalDamage > 0) {
            float dirX = 0.0f;
            if (attackBox->owner) {
                // Determine direction based on positions
                float attackerX = attackBox->owner->getWorldStats().position.x;
                float targetX = target->getWorldStats().position.x;
                dirX = (targetX > attackerX) ? 1.0f : -1.0f;
            } else if (attackBox->ignoreEntity) {
                float spawnerX = attackBox->ignoreEntity->getWorldStats().position.x;
                float targetX = target->getWorldStats().position.x;
                dirX = (targetX > spawnerX) ? 1.0f : -1.0f;
            }
            target->takeDamage(finalDamage, dirX);
        }
    }
}

void CombatSystem::renderDebug(const std::vector<Entity*>& entities) const {
    for (auto* entity : entities) {
        if (!entity->getIsActive()) continue;
        if (entity->hasActiveHitbox()) {
            Hitbox hb = entity->getActiveHitbox();
            Color color = (hb.damage > 0) ? GREEN : BLUE;
            DrawRectangleLinesEx(hb.rect, 2.0f, color);
        }
    }
}
