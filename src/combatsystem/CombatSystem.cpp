#include "CombatSystem.h"
#include "BruteForceDetector.h"
#include "Player.h"
#include "raylib.h"
#include <algorithm>
#include <iostream>

CombatSystem::CombatSystem()
    : detector(std::make_unique<BruteForceDetector>()) {}

void CombatSystem::registerPlayer(Player* p) {
    players.push_back(p);
}

void CombatSystem::update(float dt) {
    // 1. Collect active hitboxes from all players
    std::vector<Hitbox> activeHitboxes;
    for (auto* player : players) {
        if (player->hasActiveHitbox()) {
            activeHitboxes.push_back(player->getActiveHitbox());
        }
    }

    if (activeHitboxes.empty()) return;

    // 2. Run collision detection (attack hitboxes vs player physics boxes)
    std::vector<CollisionPair> collisions = detector->detect(activeHitboxes, players);

    // 3. Resolve damage for each collision
    for (const auto& pair : collisions) {
        const Hitbox* attackBox = pair.hitbox;
        Player* target = pair.target;

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
            target->takeDamage(finalDamage);
        }
    }
}

void CombatSystem::renderDebug() const {
    for (auto* player : players) {
        if (player->hasActiveHitbox()) {
            Hitbox hb = player->getActiveHitbox();
            Color color = (hb.damage > 0) ? GREEN : BLUE;
            DrawRectangleLinesEx(hb.rect, 2.0f, color);
        }
    }
}
