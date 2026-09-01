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
      spawner(spawner),
      hitboxOffsetX(config.hitboxOffsetX),
      hitboxOffsetY(config.hitboxOffsetY)
{
    faction = spawner ? spawner->getFaction() : EntityFaction::Neutral;
    
    // Load animation texture if specified
    if (!config.textureName.empty()) {
        auto& assetMgr = AssetManager::getInstance();
        const Texture2D& tex = assetMgr.getTexture(config.textureName);
        if (tex.id != 0) {
            animation = std::make_unique<Animation>(tex, config.frameNum, config.frameTime, config.scale);
        }
    }

    float autoW = config.hitboxW;
    float autoH = config.hitboxH;
    if (autoW <= 0.0f || autoH <= 0.0f) {
        if (animation) {
            Rectangle frame = animation->getCurrentFrame();
            autoW = std::abs(frame.width) * config.scale;
            autoH = std::abs(frame.height) * config.scale;
        } else {
            autoW = 16.0f;
            autoH = 16.0f;
        }
        baseStats.physicsBox = {autoW, autoH};
        runtimeStats.physicsBox = {autoW, autoH};
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

        float scale = animation->getScale();
        float absW = (source.width < 0 ? -source.width : source.width) * scale;
        float absH = source.height * scale;

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
    DrawRectangleLinesEx(getOffsetHitbox(), 1.0f, RED);
}

void Fireball::onHitWall(bool isRightWall, bool isCliff) {
    std::cout << "[Fireball] Hit wall! Deactivating. pos=(" << worldStats.position.x << ", " << worldStats.position.y << ")" << std::endl;
    isActive = false;
}

void Fireball::onCollide(Entity& other) {
    isActive = false;
}

bool Fireball::hasActiveHitbox() const {
    return isActive;
}

Rectangle Fireball::getOffsetHitbox() const {
    Rectangle baseHitbox = getHitbox();
    float offX = worldStats.isFacingRight ? hitboxOffsetX : -hitboxOffsetX;
    baseHitbox.x += offX;
    baseHitbox.y += hitboxOffsetY;
    return baseHitbox;
}

Hitbox Fireball::getActiveHitbox() {
    Rectangle rect = getOffsetHitbox();
    Hitbox hb = { rect, attackPower, 0, this, spawner };
    if (faction == EntityFaction::Player) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Enemy)) | (1 << static_cast<int>(EntityFaction::Environment));
        if (spawner && spawner->isPvPEnabled()) {
            hb.targetFactionMask |= (1 << static_cast<int>(EntityFaction::Player));
        }
    } else if (faction == EntityFaction::Enemy) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Player)) | (1 << static_cast<int>(EntityFaction::Environment));
    }
    return hb;
}

void Fireball::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    // Fireball is destroyed when it takes any damage (e.g. fireball-vs-fireball cancel)
    isActive = false;
}
