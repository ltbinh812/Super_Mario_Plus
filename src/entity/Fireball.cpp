#include "Fireball.h"
#include "AssetManager.h"
#include <iostream>
#include <cmath>

static CharacterBaseStats getFireballBaseStats(const FireballConfig& config) {
    CharacterBaseStats bs;
    bs.name = "Fireball";
    bs.moveVelocity = config.speed;
    bs.gravityScale = config.gravityScale;
    bs.physicsBox = {config.hitboxW, config.hitboxH};
    return bs;
}

static CharacterRuntimeStats getFireballRuntimeStats(bool isFacingRight, const FireballConfig& config) {
    CharacterRuntimeStats rs;
    rs.velocity.x = isFacingRight ? config.speed : -config.speed;
    rs.physicsBox = {config.hitboxW, config.hitboxH};
    return rs;
}

static CharacterWorldStats getFireballWorldStats(Vector2 startPos, bool isFacingRight) {
    CharacterWorldStats ws;
    ws.position = startPos;
    ws.isFacingRight = isFacingRight;
    ws.animation = nullptr;
    return ws;
}

Fireball::Fireball(Vector2 startPos, bool isFacingRight, const FireballConfig& config, Entity* spawner)
    : Entity(getFireballBaseStats(config), getFireballRuntimeStats(isFacingRight, config), getFireballWorldStats(startPos, isFacingRight)),
      lifetime(config.lifetime),
      attackPower(config.damage),
      curveAmplitude(config.curveAmplitude),
      curveFrequency(config.curveFrequency),
      originY(startPos.y),
      spawner(spawner)
{
    // Load animation texture if specified
    if (!config.textureName.empty()) {
        auto& assetMgr = AssetManager::getInstance();
        const Texture2D& tex = assetMgr.getTexture(config.textureName);
        if (tex.id != 0) {
            animation = std::make_unique<Animation>(tex, config.frameNum, config.frameTime);
        }
    }

    std::cout << "[Fireball] Created at (" << startPos.x << ", " << startPos.y << ") facing " << (isFacingRight ? "right" : "left")
              << " speed=" << config.speed << " damage=" << config.damage << std::endl;
}

void Fireball::update(float dt) {
    if (!isActive) return;
    
    elapsedTime += dt;
    lifetime -= dt;
    if (lifetime <= 0) {
        std::cout << "[Fireball] Expired by lifetime" << std::endl;
        isActive = false;
        return;
    }

    // Apply sine-wave curve if configured
    if (curveAmplitude > 0.0f && curveFrequency > 0.0f) {
        worldStats.position.y = originY + curveAmplitude * std::sin(curveFrequency * elapsedTime);
    }

    // Update animation if available
    if (animation) {
        animation->update(dt);
    }
}

void Fireball::render(float alpha) {
    if (!isActive) return;

    if (animation) {
        // Draw animated sprite
        Rectangle source = animation->getCurrentFrame();
        if (!worldStats.isFacingRight) {
            source.width = -source.width;
        }

        float absW = (source.width < 0 ? -source.width : source.width);
        float absH = source.height;

        // Center on position, bottom-aligned
        Rectangle dest = {
            worldStats.position.x - absW / 2.0f,
            worldStats.position.y - absH,
            absW, absH
        };

        DrawTexturePro(animation->getTexture(), source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        // Fallback: draw a circle
        DrawCircle(
            static_cast<int>(worldStats.position.x),
            static_cast<int>(worldStats.position.y - runtimeStats.physicsBox.y / 2),
            8.0f, ORANGE
        );
    }

    // Debug: draw hitbox outline
    DrawRectangleLinesEx(getHitbox(), 1.0f, RED);
}

void Fireball::onHitWall(bool isRightWall) {
    std::cout << "[Fireball] Hit wall! Deactivating. pos=(" << worldStats.position.x << ", " << worldStats.position.y << ")" << std::endl;
    isActive = false;
}

void Fireball::onCollide(Entity& other) {
    isActive = false;
}

bool Fireball::hasActiveHitbox() const {
    return isActive;
}

Hitbox Fireball::getActiveHitbox() {
    Rectangle rect = getHitbox();
    return { rect, attackPower, 0, this, spawner };
}

void Fireball::takeDamage(int damage, bool forceInterrupt) {
    // Fireball is destroyed when it takes any damage (e.g. fireball-vs-fireball cancel)
    isActive = false;
}
