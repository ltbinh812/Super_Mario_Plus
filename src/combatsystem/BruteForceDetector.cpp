#include "BruteForceDetector.h"
#include "Entity.h"
#include "raylib.h"

std::vector<CollisionPair> BruteForceDetector::detect(
    const std::vector<Hitbox>& hitboxes,
    const std::vector<Entity*>& entities
) {
    std::vector<CollisionPair> results;
    for (const auto& hb : hitboxes) {
        for (auto* entity : entities) {
            if (entity == hb.owner) continue;      // no self-hit
            if (hb.ignoreEntity && entity == hb.ignoreEntity) continue; // ignore the spawner
            if (!entity->getIsActive()) continue;   // skip dead entities
            if (CheckCollisionRecs(hb.rect, entity->getHitbox())) {
                results.push_back({&hb, entity});
            }
        }
    }
    return results;
}
