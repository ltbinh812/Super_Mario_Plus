#include "Mob.h"
#include "IMobState.h"
#include "EnemyStates/EnemyHurtState.h"
#include "EnemyStates/EnemyDieState.h"
#include "Player.h"
#include <iostream>
#include <cmath>
#include <raymath.h>

Mob::Mob(Vector2 worldPos, const std::string& type, const CharacterBaseStats& bStats, const MobConfig& cfg)
    : Entity(bStats, CharacterRuntimeStats(), CharacterWorldStats()),
      mobType(type), config(cfg), currentAnim(nullptr), stateTimer(0.0f),
      spawnPoint(worldPos), isFacingRight(false), isDead(false), hurtTimer(0.0f)
{
    faction = EntityFaction::Enemy;
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

    if (currentStandardAnim) {
        currentStandardAnim->update(dt);
    } else if (currentAnim) {
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
    // Blink effect when hurt
    if (hurtTimer > 0.0f && !isDead) {
        int blink = (int)(hurtTimer * 20.0f);
        if (blink % 2 == 0) return;
    }

    if (hasStandardAnimations() && currentStandardAnim) {
        const Texture2D& tex = currentStandardAnim->getTexture();
        if (tex.id == 0) return;

        Rectangle src = currentStandardAnim->getCurrentFrame();
        if (!isFacingRight) {
            src.width = -std::abs(src.width);
        } else {
            src.width = std::abs(src.width);
        }

        float scale = currentStandardAnim->getScale();
        float drawW = std::abs(src.width) * scale;
        float drawH = std::abs(src.height) * scale;

        Rectangle dest = {
            worldStats.position.x - drawW / 2.0f,
            worldStats.position.y - drawH,
            drawW,
            drawH
        };

        DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, WHITE);
    } else if (currentAnim && currentAnim->isValid()) {
        const Texture2D& tex = currentAnim->getTexture();
        if (tex.id == 0) return;

        Rectangle src = currentAnim->getCurrentSourceRect();
        if (!isFacingRight) {
            src.width = -std::abs(src.width);
        } else {
            src.width = std::abs(src.width);
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

    // Draw Health Bar if not dead
    if (!isDead && baseStats.maxHealth > 0) {
        float hpPercent = (float)runtimeStats.health / baseStats.maxHealth;
        if (hpPercent < 0.0f) hpPercent = 0.0f;
        
        float barWidth = 20.0f;
        float barHeight = 4.0f;
        float barX = worldStats.position.x - barWidth / 2.0f;
        float barY = worldStats.position.y + 2.0f; // Just below the mob
        
        // Background (black)
        DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);
        // Foreground (green)
        DrawRectangle((int)barX, (int)barY, (int)(barWidth * hpPercent), (int)barHeight, GREEN);
    }
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

Player* Mob::getClosestPlayer() const {
    Player* closest = nullptr;
    float minDistance = 999999.0f;
    Vector2 myPos = getPosition();

    for (Player* p : targetPlayers) {
        if (p && !p->isDead()) {
            float dist = Vector2Distance(myPos, p->getPosition());
            if (dist < minDistance) {
                minDistance = dist;
                closest = p;
            }
        }
    }
    return closest;
}

void Mob::takeDamage(int damage, float knockbackDirX, bool forceInterrupt) {
    if (isDead || hurtTimer > 0.0f) return;

    runtimeStats.health -= damage;

    if (runtimeStats.health <= 0) {
        runtimeStats.health = 0;
        isDead = true;
        changeState(std::make_unique<EnemyDieState>());
    } else if (forceInterrupt) {
        changeState(std::make_unique<EnemyHurtState>(knockbackDirX, 0.5f));
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
    if (hasStandardAnimations()) {
        auto it = standardAnimations.find(animName);
        if (it != standardAnimations.end()) {
            if (currentStandardAnim != &it->second) {
                currentStandardAnim = &it->second;
                currentStandardAnim->resetAnimation();
            }
        }
    } else {
        auto it = animations.find(animName);
        if (it != animations.end()) {
            if (currentAnim != &it->second) {
                currentAnim = &it->second;
                currentAnim->reset();
            }
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
