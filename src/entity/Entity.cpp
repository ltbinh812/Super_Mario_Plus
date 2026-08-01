#include "Entity.h"
#include "TileMap.h"
#include <cmath>

Entity::Entity(CharacterBaseStats &bS, CharacterRuntimeStats &rS,
               CharacterWorldStats &wS)
    : baseStats(bS), runtimeStats(rS), worldStats(wS) {}

Rectangle Entity::getHitbox() const {
    return { worldStats.position.x - runtimeStats.physicsBox.x/ 2.0f, worldStats.position.y - runtimeStats.physicsBox.y, runtimeStats.physicsBox.x, runtimeStats.physicsBox.y };
}

void Entity::updatePhysicsWithMap(const TileMap& map, float dt) {
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

    std::vector<Rectangle> collidersX = map.GetCollidingRectangles(hitBoxX);
    if (!collidersX.empty()) {
        for (const Rectangle& rect : collidersX) {
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

    std::vector<Rectangle> collidersY = map.GetCollidingRectangles(hitBoxY);

    if (!collidersY.empty()) {
        for (const Rectangle& rect : collidersY) {
            Rectangle currentHitBoxY = getHitbox();
            currentHitBoxY.x += SHRINK;
            currentHitBoxY.width -= SHRINK * 2;
            
            if (!CheckCollisionRecs(currentHitBoxY, rect)) continue;

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
        }
    } else {
        // Check if grounded by testing 1px below feet (e.g. walking off ledge)
        Rectangle feetTest = getHitbox();
        feetTest.y += 1.0f;
        // Cần thu nhỏ X của feet test giống như hitBoxY để tránh bám tường ảo
        feetTest.x += SHRINK;
        feetTest.width -= SHRINK * 2;
        
        std::vector<Rectangle> groundCheck = map.GetCollidingRectangles(feetTest);
        if (groundCheck.empty() || runtimeStats.velocity.y < 0.0f) {
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
