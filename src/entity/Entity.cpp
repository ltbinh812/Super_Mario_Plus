#include "Entity.h"
#include "TileMap.h"
#include <cmath>
#include <algorithm>

Entity::Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
               const CharacterWorldStats &wS)
    : baseStats(bS), runtimeStats(rS), worldStats(wS) {}

Rectangle Entity::getHitbox() const {
    return { worldStats.position.x - runtimeStats.physicsBox.x/ 2.0f, worldStats.position.y - runtimeStats.physicsBox.y, runtimeStats.physicsBox.x, runtimeStats.physicsBox.y };
}

void Entity::dropThrough() {
    runtimeStats.ignoreOneWayTimer = 0.2f;
}

void Entity::addEffect(std::unique_ptr<IEffect> effect) {
    for (auto& eff : activeEffects) {
        if (eff->getName() == effect->getName()) {
            eff->refresh();
            return;
        }
    }
    activeEffects.push_back(std::move(effect));
}

bool Entity::hasEffect(const std::string& name) const {
    for (const auto& eff : activeEffects) {
        if (eff->getName() == name) return true;
    }
    return false;
}

void Entity::updateEffects(float dt) {
    for (auto it = activeEffects.begin(); it != activeEffects.end();) {
        if ((*it)->update(*this, dt)) {
            it = activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

void Entity::clearEffects() {
    activeEffects.clear();
}

void Entity::handleTriggers(const TileMap& map, float dt) {
    runtimeStats.isOverlappingLadder = false;
    
    Rectangle triggerHitbox = getHitbox();
    triggerHitbox.height += 4.0f; 

    std::vector<CollisionTile> currentTiles = map.GetCollidingTiles(triggerHitbox);

    bool inLiquid = false;
    float highestLiquidY = 999999.0f;
    CollisionType dominantLiquid = CollisionType::None;

    for (const auto& tile : currentTiles) {
        if (tile.type == CollisionType::Ladder) {
            runtimeStats.isOverlappingLadder = true;
            onOverlapLadder();
        }
        if (tile.type == CollisionType::Hazard) onHazard();
        if (tile.type == CollisionType::Die) {
            if (runtimeStats.health > 0) takeDamage(9999);
        }

        if (tile.type == CollisionType::Water || tile.type == CollisionType::Poison || tile.type == CollisionType::Lava) {
            inLiquid = true;
            if (tile.rect.y < highestLiquidY) highestLiquidY = tile.rect.y;
            dominantLiquid = tile.type; // Last detected liquid type (could be prioritized if needed)
        }
    }
    
    if (inLiquid) {
        runtimeStats.currentLiquid = dominantLiquid;
        // If the top of the player is higher than the liquid surface, they are partially outside
        if (triggerHitbox.y < highestLiquidY) {
            runtimeStats.isPartiallyOutsideLiquid = true;
        } else {
            runtimeStats.isPartiallyOutsideLiquid = false;
        }

        if (dominantLiquid == CollisionType::Poison) {
            addEffect(std::make_unique<PoisonEffect>());
        } else if (dominantLiquid == CollisionType::Lava) {
            addEffect(std::make_unique<LavaEffect>());
        } else if (dominantLiquid == CollisionType::Water) {
            for (auto it = activeEffects.begin(); it != activeEffects.end(); ) {
                if ((*it)->getName() == "Lava") {
                    it = activeEffects.erase(it);
                } else {
                    ++it;
                }
            }
        }

        onEnterWater(); // Enter generic liquid state
    } else {
        runtimeStats.currentLiquid = CollisionType::None;
        runtimeStats.isPartiallyOutsideLiquid = true;
        onExitLiquid(); // Custom virtual method to leave swim state
    }

    for (auto& eff : activeEffects) {
        if (eff->getName() == "Lava") {
            bool foundLava = (runtimeStats.currentLiquid == CollisionType::Lava);
            static_cast<LavaEffect*>(eff.get())->setInLava(foundLava);
        }
        if (eff->getName() == "Poison") {
            bool foundPoison = (runtimeStats.currentLiquid == CollisionType::Poison);
            static_cast<PoisonEffect*>(eff.get())->setInPoison(foundPoison);
        }
    }
}

void Entity::applyGravity(float dt) {
    float mod = 1.0f;
    if (runtimeStats.currentLiquid == CollisionType::Water ||
        runtimeStats.currentLiquid == CollisionType::Poison ||
        runtimeStats.currentLiquid == CollisionType::Lava) {
        mod = 0.4f; // Slower falling in liquid
    }
    runtimeStats.velocity.y += baseStats.gravityScale * mod * 9.8f * dt;
    if (mod < 1.0f && runtimeStats.velocity.y > 40.0f) {
        // Rơi ngập 3/4 độ cao: deceleration = 1.733 * g
        runtimeStats.velocity.y -= baseStats.gravityScale * 9.8f * 1.733f * dt;
        if (runtimeStats.velocity.y < 40.0f) {
            runtimeStats.velocity.y = 40.0f;
        }
    }
}

void Entity::resolveCollisionX(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt) {
    const float EPSILON = 0.01f;
    const float SHRINK = 0.2f;
    worldStats.position.x += runtimeStats.velocity.x * dt;
    Rectangle hitBoxX = getHitbox();
    hitBoxX.y += SHRINK;
    hitBoxX.height -= SHRINK * 2;

    std::vector<CollisionTile> collidersX = map.GetCollidingTiles(hitBoxX);
    for (const auto& r : dynamicSolids) {
        if (CheckCollisionRecs(hitBoxX, r)) {
            collidersX.push_back({r, CollisionType::Solid});
        }
    }
    if (!collidersX.empty()) {
        for (const auto& tile : collidersX) {
            if ((runtimeStats.collisionMask & (1 << (int)tile.type)) == 0) continue;
            // X-axis only stops on Solid and Cloud
            if (tile.type != CollisionType::Solid && tile.type != CollisionType::Cloud) continue;

            const Rectangle& rect = tile.rect;
            Rectangle currentHitBoxX = getHitbox();
            currentHitBoxX.y += SHRINK;
            currentHitBoxX.height -= SHRINK * 2;
            
            if (!CheckCollisionRecs(currentHitBoxX, rect)) continue;

            if (tile.type == CollisionType::Solid || tile.type == CollisionType::Cloud) {
                if (runtimeStats.velocity.x > 0) { 
                    worldStats.position.x = rect.x - (runtimeStats.physicsBox.x / 2.0f) - EPSILON;
                    if (tile.type == CollisionType::Cloud && std::abs(runtimeStats.velocity.x) > 350.0f) {
                        runtimeStats.velocity.x = -runtimeStats.velocity.x * 1.5f; // Stronger bounce for dash
                    } else {
                        runtimeStats.velocity.x = 0.0f;
                        onHitWall(true);
                    }
                } else if (runtimeStats.velocity.x < 0) { 
                    worldStats.position.x = rect.x + rect.width + (runtimeStats.physicsBox.x / 2.0f) + EPSILON;
                    if (tile.type == CollisionType::Cloud && runtimeStats.velocity.x < -350.0f) {
                        runtimeStats.velocity.x = -runtimeStats.velocity.x * 1.5f; // Stronger bounce for dash
                    } else {
                        runtimeStats.velocity.x = 0.0f;
                        onHitWall(false);
                    }
                }
            }
        }
    }
}

void Entity::resolveCollisionY(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt) {
    const float EPSILON = 0.01f;
    const float SHRINK = 0.2f;
    worldStats.position.y += runtimeStats.velocity.y * dt;
    Rectangle hitBoxY = getHitbox();
    hitBoxY.x += SHRINK;
    hitBoxY.width -= SHRINK * 2;

    std::vector<CollisionTile> collidersY = map.GetCollidingTiles(hitBoxY);
    for (const auto& r : dynamicSolids) {
        if (CheckCollisionRecs(hitBoxY, r)) {
            collidersY.push_back({r, CollisionType::Solid});
        }
    }

    if (!collidersY.empty()) {
        for (const auto& tile : collidersY) {
            if ((runtimeStats.collisionMask & (1 << (int)tile.type)) == 0) continue;
            
            const Rectangle& rect = tile.rect;
            Rectangle currentHitBoxY = getHitbox();
            currentHitBoxY.x += SHRINK;
            currentHitBoxY.width -= SHRINK * 2;
            
            if (!CheckCollisionRecs(currentHitBoxY, rect)) continue;

            if (tile.type == CollisionType::Solid || tile.type == CollisionType::Cloud) {
                if (runtimeStats.velocity.y >= 0) { 
                    worldStats.position.y = rect.y - EPSILON;
                    if (tile.type == CollisionType::Cloud && runtimeStats.velocity.y > 250.0f) {
                        runtimeStats.velocity.y = -runtimeStats.velocity.y * 0.75f;
                    } else {
                        runtimeStats.velocity.y = 0.0f;
                        runtimeStats.isGrounded = true;
                        onLand(rect.y);
                    }
                } else if (runtimeStats.velocity.y < 0) { 
                    worldStats.position.y = rect.y + rect.height + runtimeStats.physicsBox.y + EPSILON;
                    if (tile.type == CollisionType::Cloud) {
                        runtimeStats.velocity.y = -runtimeStats.velocity.y * 0.75f;
                    } else {
                        runtimeStats.velocity.y = 0.0f;
                        onHitCeiling(rect.y + rect.height);
                    }
                }
            } else if (tile.type == CollisionType::OneWay || tile.type == CollisionType::Lotus) {
                if (runtimeStats.velocity.y > 0) { 
                    float previousBottom = worldStats.position.y - runtimeStats.velocity.y * dt;
                    if (previousBottom <= rect.y + 1.0f) {
                        worldStats.position.y = rect.y - EPSILON;
                        runtimeStats.velocity.y = 0.0f;
                        runtimeStats.isGrounded = true;
                        onLand(rect.y);
                    }
                }
            } else if (tile.type == CollisionType::Slop) {
                float playerX = worldStats.position.x;
                float clampedX = std::max(rect.x, std::min(playerX, rect.x + rect.width));
                float surfaceY = rect.y + rect.height - (clampedX - rect.x);
                if (worldStats.position.y >= surfaceY - EPSILON && runtimeStats.velocity.y >= 0) {
                    worldStats.position.y = surfaceY - EPSILON;
                    runtimeStats.velocity.y = 0.0f;
                    runtimeStats.isGrounded = true;
                    onLand(surfaceY);
                }
            }
        }
    } 

    // Ground check
    Rectangle feetTest = getHitbox();
    feetTest.y += 1.0f;
    feetTest.x += SHRINK;
    feetTest.width -= SHRINK * 2;
    
    std::vector<CollisionTile> groundCheck = map.GetCollidingTiles(feetTest);
    for (const auto& r : dynamicSolids) {
        if (CheckCollisionRecs(feetTest, r)) {
            groundCheck.push_back({r, CollisionType::Solid});
        }
    }
    bool foundGround = false;
    for (const auto& tile : groundCheck) {
        if ((runtimeStats.collisionMask & (1 << (int)tile.type)) != 0) {
            if (tile.type == CollisionType::Solid || tile.type == CollisionType::Cloud) {
                foundGround = true;
                break;
            } else if (tile.type == CollisionType::OneWay || tile.type == CollisionType::Lotus) {
                if (worldStats.position.y <= tile.rect.y + 2.0f) {
                    foundGround = true;
                    break;
                }
            } else if (tile.type == CollisionType::Slop) {
                float playerX = worldStats.position.x;
                float clampedX = std::max(tile.rect.x, std::min(playerX, tile.rect.x + tile.rect.width));
                float surfaceY = tile.rect.y + tile.rect.height - (clampedX - tile.rect.x);
                if (worldStats.position.y >= surfaceY - EPSILON - 2.0f) {
                    foundGround = true;
                    break;
                }
            }
        }
    }
    if (!foundGround || runtimeStats.velocity.y < 0.0f) {
        runtimeStats.isGrounded = false;
    }
}

void Entity::updatePhysicsWithMap(const TileMap& map, const std::vector<Rectangle>& dynamicSolids, float dt) {
    if (runtimeStats.ignoreOneWayTimer > 0.0f) {
        runtimeStats.ignoreOneWayTimer -= dt;
        runtimeStats.collisionMask &= ~(1 << (int)CollisionType::OneWay);
        runtimeStats.collisionMask &= ~(1 << (int)CollisionType::Lotus);
    } else {
        runtimeStats.collisionMask |= (1 << (int)CollisionType::OneWay);
        runtimeStats.collisionMask |= (1 << (int)CollisionType::Lotus);
    }
    if (runtimeStats.ignoreLadderTimer > 0.0f) {
        runtimeStats.ignoreLadderTimer -= dt;
    }

    handleTriggers(map, dt);
    applyGravity(dt);
    resolveCollisionX(map, dynamicSolids, dt);
    resolveCollisionY(map, dynamicSolids, dt);
}

void Entity::updatePhysicsSimple(float groundY, float dt) {
    float mod = 1.0f;
    if (runtimeStats.currentLiquid == CollisionType::Water ||
        runtimeStats.currentLiquid == CollisionType::Poison ||
        runtimeStats.currentLiquid == CollisionType::Lava) {
        mod = 0.4f;
    }
    runtimeStats.velocity.y += baseStats.gravityScale * mod * 9.8f * dt;
    if (mod < 1.0f && runtimeStats.velocity.y > 40.0f) {
        // Rơi ngập 3/4 độ cao: deceleration = 1.733 * g
        runtimeStats.velocity.y -= baseStats.gravityScale * 9.8f * 1.733f * dt;
        if (runtimeStats.velocity.y < 40.0f) {
            runtimeStats.velocity.y = 40.0f;
        }
    }
    worldStats.position.x += runtimeStats.velocity.x * dt;
    worldStats.position.y += runtimeStats.velocity.y * dt;
    if (worldStats.position.y >= groundY) {
        worldStats.position.y = groundY;
        runtimeStats.velocity.y = 0.0f;
        runtimeStats.isGrounded = true;
        onLand(groundY);
    } else {
        runtimeStats.isGrounded = false;
    }
}
