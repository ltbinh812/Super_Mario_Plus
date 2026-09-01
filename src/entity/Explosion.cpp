#include "Explosion.h"
#include "AssetManager.h"
#include <raylib.h>

static CharacterBaseStats getExplosionBaseStats(const ExplosionConfig& config) {
    CharacterBaseStats bs;
    bs.name = "Explosion";
    bs.moveVelocity = 0.0f;
    bs.gravityScale = 0.0f;
    bs.physicsBox = {config.hitboxW, config.hitboxH};
    return bs;
}

static CharacterRuntimeStats getExplosionRuntimeStats(const ExplosionConfig& config) {
    CharacterRuntimeStats rs;
    rs.velocity = {0.0f, 0.0f};
    rs.physicsBox = {config.hitboxW, config.hitboxH};
    return rs;
}

static CharacterWorldStats getExplosionWorldStats(Vector2 startPos, bool isFacingRight) {
    CharacterWorldStats ws;
    ws.position = startPos;
    ws.isFacingRight = isFacingRight;
    ws.animation = nullptr;
    return ws;
}

Explosion::Explosion(Vector2 startPos, bool isFacingRight, const ExplosionConfig& config, Entity* spawner)
    : Entity(getExplosionBaseStats(config), getExplosionRuntimeStats(config), getExplosionWorldStats(startPos, isFacingRight)),
      lifetime(config.lifetime), attackPower(config.damage), 
      hitboxW(config.hitboxW), hitboxH(config.hitboxH), spawner(spawner), onHitEffect(config.onHitEffect),
      hitboxStartFrame(config.hitboxStartFrame), hitboxEndFrame(config.hitboxEndFrame),
      hitboxOffsetX(config.hitboxOffsetX), hitboxOffsetY(config.hitboxOffsetY) {
    
    faction = spawner ? spawner->getFaction() : EntityFaction::Neutral;
    
    if (!config.textureName.empty()) {
        auto& assetMgr = AssetManager::getInstance();
        const Texture2D& tex = assetMgr.getTexture(config.textureName);
        if (tex.id != 0) {
            animation = std::make_unique<Animation>(tex, config.frameNum, config.frameTime, config.scale);
        }
    }
    
    if (hitboxW <= 0.0f || hitboxH <= 0.0f) {
        if (animation) {
            Rectangle frame = animation->getCurrentFrame();
            hitboxW = std::abs(frame.width) * config.scale;
            hitboxH = std::abs(frame.height) * config.scale;
        } else {
            hitboxW = 100.0f;
            hitboxH = 100.0f;
        }
        baseStats.physicsBox = {hitboxW, hitboxH};
        runtimeStats.physicsBox = {hitboxW, hitboxH};
    }
}

void Explosion::update(float dt) {
    if (!isActive) return;
    lifetime -= dt;
    if (lifetime <= 0) {
        isActive = false;
        return;
    }
    
    if (animation) {
        animation->update(dt);
    }
}

void Explosion::render(float alpha) {
    if (!isActive) return;

    if (animation) {
        Rectangle source = animation->getCurrentFrame();
        if (!worldStats.isFacingRight) {
            source.width = -source.width;
        }
        float scale = animation->getScale();
        float absW = (source.width < 0 ? -source.width : source.width) * scale;
        float absH = source.height * scale;

        Rectangle dest = {
            worldStats.position.x - absW / 2.0f,
            worldStats.position.y - absH,
            absW, absH
        };

        DrawTexturePro(animation->getTexture(), source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        DrawCircleLines(
            static_cast<int>(worldStats.position.x),
            static_cast<int>(worldStats.position.y - hitboxH / 2.0f),
            hitboxW / 2.0f, RED
        );
    }
    DrawRectangleLinesEx(getActiveHitbox().rect, 1.0f, ORANGE);
}

bool Explosion::hasActiveHitbox() const {
    if (!isActive) return false;
    if (animation) {
        int currentFrame = animation->getCurrentFrameIndex();
        return currentFrame >= hitboxStartFrame && currentFrame <= hitboxEndFrame;
    }
    return true;
}

Rectangle Explosion::getOffsetHitbox() const {
    Rectangle rect = { worldStats.position.x - hitboxW / 2.0f, worldStats.position.y - hitboxH, hitboxW, hitboxH };
    float offX = worldStats.isFacingRight ? hitboxOffsetX : -hitboxOffsetX;
    rect.x += offX;
    rect.y += hitboxOffsetY;
    return rect;
}

Hitbox Explosion::getActiveHitbox() {
    Rectangle rect = getOffsetHitbox();
    Hitbox hb = { rect, attackPower, 0, this, spawner, onHitEffect };
    if (faction == EntityFaction::Player) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Enemy)) | (1 << static_cast<int>(EntityFaction::Environment));
    } else if (faction == EntityFaction::Enemy) {
        hb.targetFactionMask = (1 << static_cast<int>(EntityFaction::Player)) | (1 << static_cast<int>(EntityFaction::Environment));
    }
    return hb;
}
