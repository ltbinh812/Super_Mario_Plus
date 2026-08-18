#include "Entity.h"
#include "TileMap.h"
#include <cmath>

Entity::Entity(const CharacterBaseStats &bS, const CharacterRuntimeStats &rS,
               const CharacterWorldStats &wS)
    : baseStats(bS), runtimeStats(rS), worldStats(wS) {}

Rectangle Entity::getHitbox() const {
    return { worldStats.position.x - runtimeStats.physicsBox.x/ 2.0f, worldStats.position.y - runtimeStats.physicsBox.y, runtimeStats.physicsBox.x, runtimeStats.physicsBox.y };
}

void Entity::dropThrough() {
    runtimeStats.ignoreOneWayTimer = 0.2f;
}

void Entity::updatePhysicsWithMap(const TileMap& map, float dt) {
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


    runtimeStats.isOverlappingLadder = false;

    // Environmental Overlaps (Triggers)
    Rectangle triggerHitbox = getHitbox();
    // Mở rộng hitbox ra một chút (đặc biệt là hướng xuống) 
    // để có thể nhận diện thang (Ladder) ngay cả khi đang đứng ở block liền kề bên trên
    triggerHitbox.height += 4.0f; 

    std::vector<CollisionTile> currentTiles = map.GetCollidingTiles(triggerHitbox);

    for (const auto& tile : currentTiles) {
        if (tile.type == CollisionType::Water) onEnterWater();
        if (tile.type == CollisionType::Ladder) {
            runtimeStats.isOverlappingLadder = true;
            onOverlapLadder();
        }
        if (tile.type == CollisionType::Hazard) onHazard();
        if (tile.type == CollisionType::Die) onDie();
    }


    // 1. Apply Gravity

    runtimeStats.velocity.y += baseStats.gravityScale * 9.8f * dt;

    // Lấy một lượng epsilon nhỏ để bù trừ sai số dấu phẩy động
    const float EPSILON = 0.01f;
    const float SHRINK = 0.2f;

    // 2. Axis-Separated X Resolution (Horizontal Movement & Wall Collision)
    worldStats.position.x += runtimeStats.velocity.x * dt;
    Rectangle hitBoxX = getHitbox();
    
    // Thu nhỏ hitbox theo trục Y một chút để khi di chuyển ngang không bị vướng vào sàn/trần
    hitBoxX.y += SHRINK;
    hitBoxX.height -= SHRINK * 2;

    std::vector<CollisionTile> collidersX = map.GetCollidingTiles(hitBoxX);
    if (!collidersX.empty()) {
        for (const auto& tile : collidersX) {
            // Check mask
            if ((runtimeStats.collisionMask & (1 << (int)tile.type)) == 0) continue;
            // X-axis only stops on Solid
            if (tile.type != CollisionType::Solid) continue;

            const Rectangle& rect = tile.rect;

            // Tính toán lại hitbox hiện tại vì nó có thể đã bị thay đổi bởi lần lặp trước
            Rectangle currentHitBoxX = getHitbox();
            currentHitBoxX.y += SHRINK;
            currentHitBoxX.height -= SHRINK * 2;
            
            if (!CheckCollisionRecs(currentHitBoxX, rect)) continue;

            if (runtimeStats.velocity.x > 0) { // Moving right -> hit left wall of block
                worldStats.position.x = rect.x - (runtimeStats.physicsBox.x / 2.0f) - EPSILON;
                runtimeStats.velocity.x = 0.0f;
                onHitWall(true);
            } else if (runtimeStats.velocity.x < 0) { // Moving left -> hit right wall of block
                worldStats.position.x = rect.x + rect.width + (runtimeStats.physicsBox.x / 2.0f) + EPSILON;
                runtimeStats.velocity.x = 0.0f;
                onHitWall(false);
            }
        }
    }


    // 3. Axis-Separated Y Resolution (Vertical Movement & Floor/Ceiling Collision)
    worldStats.position.y += runtimeStats.velocity.y * dt;
    Rectangle hitBoxY = getHitbox();
    
    // Thu nhỏ hitbox theo trục X một chút để khi nhảy/rơi không bị vướng vào tường bên cạnh
    hitBoxY.x += SHRINK;
    hitBoxY.width -= SHRINK * 2;

    std::vector<CollisionTile> collidersY = map.GetCollidingTiles(hitBoxY);

    if (!collidersY.empty()) {
        for (const auto& tile : collidersY) {
            if ((runtimeStats.collisionMask & (1 << (int)tile.type)) == 0) continue;
            
            const Rectangle& rect = tile.rect;
            Rectangle currentHitBoxY = getHitbox();
            currentHitBoxY.x += SHRINK;
            currentHitBoxY.width -= SHRINK * 2;
            
            if (!CheckCollisionRecs(currentHitBoxY, rect)) continue;

            if (tile.type == CollisionType::Solid) {
                if (runtimeStats.velocity.y >= 0) { // Falling down -> landed on floor
                    worldStats.position.y = rect.y - EPSILON;
                    runtimeStats.velocity.y = 0.0f;
                    runtimeStats.isGrounded = true;
                    onLand(rect.y);
                } else if (runtimeStats.velocity.y < 0) { // Jumping up -> hit ceiling
                    worldStats.position.y = rect.y + rect.height + runtimeStats.physicsBox.y + EPSILON;
                    runtimeStats.velocity.y = 0.0f;
                    onHitCeiling(rect.y + rect.height);
                }
            } else if (tile.type == CollisionType::OneWay || tile.type == CollisionType::Lotus) {
                if (runtimeStats.velocity.y > 0) { // Only collide if falling
                    float previousBottom = worldStats.position.y - runtimeStats.velocity.y * dt;
                    if (previousBottom <= rect.y + 1.0f) {
                        worldStats.position.y = rect.y - EPSILON;
                        runtimeStats.velocity.y = 0.0f;
                        runtimeStats.isGrounded = true;
                        onLand(rect.y);
                    }
                }
            }
        }
    } else {
        // Check if grounded by testing 1px below feet (e.g. walking off ledge)
        Rectangle feetTest = getHitbox();
        feetTest.y += 1.0f;
        // Cần thu nhỏ X của feet test giống như hitBoxY để tránh bám tường ảo
        feetTest.x += SHRINK;
        feetTest.width -= SHRINK * 2;
        
        std::vector<CollisionTile> groundCheck = map.GetCollidingTiles(feetTest);
        bool foundGround = false;
        for (const auto& tile : groundCheck) {
            if ((runtimeStats.collisionMask & (1 << (int)tile.type)) != 0) {
                if (tile.type == CollisionType::Solid) {
                    foundGround = true;
                    break;
                } else if (tile.type == CollisionType::OneWay || tile.type == CollisionType::Lotus) {
                    // Đối với OneWay, chỉ tính là chạm đất nếu chân người chơi thực sự nằm 
                    // Ở TRÊN mặt (hoặc xê xích rất nhỏ), chứ không phải đang chui giữa block.
                    if (worldStats.position.y <= tile.rect.y + 2.0f) {
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

}

void Entity::updatePhysicsSimple(float groundY, float dt) {
    runtimeStats.velocity.y += baseStats.gravityScale * 9.8f * dt;
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

void Entity::addFloatingText(const std::string& text, Color color, Vector2 offset, float lifetime) {
    FloatingText ft;
    ft.text = text;
    ft.position = {worldStats.position.x + offset.x, worldStats.position.y - runtimeStats.physicsBox.y + offset.y};
    ft.velocity = {0, -50.0f}; // Float upwards
    ft.color = color;
    ft.lifetime = lifetime;
    ft.maxLifetime = lifetime;
    worldStats.floatingTexts.push_back(ft);
}

void Entity::updateFloatingTexts(float dt) {
    for (auto it = worldStats.floatingTexts.begin(); it != worldStats.floatingTexts.end(); ) {
        it->position.x += it->velocity.x * dt;
        it->position.y += it->velocity.y * dt;
        it->lifetime -= dt;
        if (it->lifetime <= 0.0f) {
            it = worldStats.floatingTexts.erase(it);
        } else {
            ++it;
        }
    }
}

void Entity::renderFloatingTexts() {
    for (const auto& ft : worldStats.floatingTexts) {
        // Fade out
        float alpha = ft.lifetime / ft.maxLifetime;
        Color c = ft.color;
        c.a = static_cast<unsigned char>(255 * alpha);
        
        DrawText(ft.text.c_str(), static_cast<int>(ft.position.x), static_cast<int>(ft.position.y), 10, c);
    }
}
