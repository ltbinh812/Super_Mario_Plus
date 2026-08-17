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

static CharacterWorldStats getExplosionWorldStats(Vector2 startPos) {
    CharacterWorldStats ws;
    ws.position = startPos;
    ws.isFacingRight = true;
    ws.animation = nullptr;
    return ws;
}

Explosion::Explosion(Vector2 startPos, const ExplosionConfig& config, Entity* spawner)
    : Entity(getExplosionBaseStats(config), getExplosionRuntimeStats(config), getExplosionWorldStats(startPos)),
      lifetime(config.lifetime), attackPower(config.damage), 
      hitboxW(config.hitboxW), hitboxH(config.hitboxH), spawner(spawner) {
    
    if (!config.textureName.empty()) {
        auto& assetMgr = AssetManager::getInstance();
        const Texture2D& tex = assetMgr.getTexture(config.textureName);
        if (tex.id != 0) {
            animation = std::make_unique<Animation>(tex, config.frameNum, config.frameTime);
        }
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
        float absW = (source.width < 0 ? -source.width : source.width);
        float absH = source.height;

        Rectangle dest = {
            worldStats.position.x - absW / 2.0f,
            worldStats.position.y - absH / 2.0f,
            absW, absH
        };

        DrawTexturePro(animation->getTexture(), source, dest, {0, 0}, 0.0f, WHITE);
    } else {
        DrawCircleLines(
            static_cast<int>(worldStats.position.x),
            static_cast<int>(worldStats.position.y),
            hitboxW / 2.0f, RED
        );
    }
    DrawRectangleLinesEx(getActiveHitbox().rect, 1.0f, ORANGE);
}

bool Explosion::hasActiveHitbox() const {
    return isActive;
}

Hitbox Explosion::getActiveHitbox() {
    Rectangle rect = { worldStats.position.x - hitboxW / 2.0f, worldStats.position.y - hitboxH / 2.0f, hitboxW, hitboxH };
    return { rect, attackPower, 0, this, spawner };
}
