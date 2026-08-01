#include "BruteForceDetector.h"
#include "Player.h"
#include "raylib.h"

std::vector<CollisionPair> BruteForceDetector::detect(
    const std::vector<Hitbox>& hitboxes,
    const std::vector<Player*>& entities
) {
    std::vector<CollisionPair> results;
    for (const auto& hb : hitboxes) {
        for (auto* player : entities) {
            if (player == hb.owner) continue;  // no self-hit
            if (CheckCollisionRecs(hb.rect, player->getHitbox())) {
                results.push_back({&hb, player});
            }
        }
    }
    return results;
}
