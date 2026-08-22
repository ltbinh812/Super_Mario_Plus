#include "Mob.h"
#include "IMobState.h"
#include <iostream>
#include <cmath>

Mob::Mob(Vector2 worldPos, const std::string& type, const CharacterBaseStats& bStats)
    : Entity(bStats, CharacterRuntimeStats(), CharacterWorldStats()),
      mobType(type), currentAnim(nullptr), stateTimer(0.0f),
      spawnPoint(worldPos), isFacingRight(false), isDead(false), hurtTimer(0.0f)
{
    worldStats.position = worldPos;
    runtimeStats.health = bStats.maxHealth;
    runtimeStats.physicsBox = bStats.physicsBox;
    TraceLog(LOG_INFO, "[Mob] Created %s at (%f,%f) with health: %d", mobType.c_str(), worldPos.x, worldPos.y, runtimeStats.health);
}

Mob::~Mob() {
    if (currentState) {
        currentState->exit(*this);
    }
}

void Mob::update(float dt) {
    if (isDead) {
        if (currentAnim) currentAnim->update(dt);
        return;
    }

    if (hurtTimer > 0.0f) {
        hurtTimer -= dt;
    }

    if (currentAnim) {
        currentAnim->update(dt);
    }
    
    // Debug pos periodically
    static float logT = 0.0f;
    logT += dt;
    if (logT > 1.0f) {
        TraceLog(LOG_INFO, "[Mob] %s is at (%f, %f) Active: %d", mobType.c_str(), worldStats.position.x, worldStats.position.y, getIsActive());
        logT = 0.0f;
    }
}

void Mob::render(float alpha) {
    if (!currentAnim || !currentAnim->isValid()) return;

    // Blink effect when hurt
    if (hurtTimer > 0.0f && !isDead) {
        int blink = (int)(hurtTimer * 20.0f);
        if (blink % 2 == 0) return;
    }

    const Texture2D& tex = currentAnim->getTexture();
    if (tex.id == 0) {
        TraceLog(LOG_ERROR, "[Mob] ERROR: Texture ID is 0 for %s!", mobType.c_str());
        return;
    }

    Rectangle src = currentAnim->getCurrentSourceRect();
    if (!isFacingRight) {
        src.width = -src.width;
    }

    float drawW = std::abs(src.width);
    float drawH = std::abs(src.height);

    Rectangle dest = {
        worldStats.position.x - drawW / 2.0f,
        worldStats.position.y - drawH,
        drawW,
        drawH
    };

    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
}

void Mob::decideAction() {
    if (isDead || hurtTimer > 0.0f) return;
    if (currentState) {
        currentState->decideAction(*this);
    }
}

void Mob::process() {
    if (isDead) {
        // If dead animation finished, we could mark for deletion
        return;
    }
    if (currentState) {
        currentState->process(*this);
    }
}

void Mob::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    if (isDead || hurtTimer > 0.0f) return;

    runtimeStats.health -= damage;
    hurtTimer = 0.5f;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        setAnimation(mobType + "_die");
        runtimeStats.velocity.x = 0;
    } else if (forceInterrupt) {
        setAnimation(mobType + "_hurt");
        runtimeStats.velocity.x = knockbackDirX * 100.0f;
        runtimeStats.velocity.y = -150.0f;
    }
}

bool Mob::getIsActive() const {
    // If dead, we might keep it active until animation finishes, but for simplicity:
    return !isDead || (currentAnim && !currentAnim->isFinished());
}

void Mob::changeState(std::unique_ptr<IMobState> newState) {
    if (currentState) {
        currentState->exit(*this);
    }
    currentState = std::move(newState);
    stateTimer = 0.0f;
    if (currentState) {
        currentState->enter(*this);
    }
}

void Mob::setAnimation(const std::string& animName) {
    auto it = animations.find(animName);
    if (it != animations.end()) {
        if (currentAnim != &it->second) {
            currentAnim = &it->second;
            currentAnim->reset();
        }
    }
}

void Mob::onHitWall(bool rightWall) {
    if (rightWall && isFacingRight) {
        isFacingRight = false;
        runtimeStats.velocity.x = -std::abs(runtimeStats.velocity.x);
    } else if (!rightWall && !isFacingRight) {
        isFacingRight = true;
        runtimeStats.velocity.x = std::abs(runtimeStats.velocity.x);
    }
}

void Mob::onLand(float floorY) {
    runtimeStats.velocity.y = 0.0f;
}
