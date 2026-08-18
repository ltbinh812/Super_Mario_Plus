#include "Boom.h"
#include "Entity.h"
#include "Player.h"
#include "CommandQueue.h"
#include "SpawnCommand.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

static const float BLOCK_SIZE = 32.0f;

// ─── Constructors ───────────────────────────────────────────────────────────

Boom::Boom(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    // Pop up when spawned from a chest/luckyblock
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };
}

// Pre-activated thrown bomb: starts counting down immediately
Boom::Boom(Vector2 worldPos, Vector2 initVelocity)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = initVelocity;
    activate();
}

// ─── Lifecycle ──────────────────────────────────────────────────────────────

void Boom::activate() {
    if (active_ || exploded_) return;
    active_ = true;
    timer_  = EXPLODE_TIME;
    std::cout << "[Boom] Countdown started!\n";
}

void Boom::update(float dt) {
    BaseItem::update(dt);

    if (exploded_) {
        // Push ExplosionDamage command exactly once so Process() can apply area burn
        if (!damageEmitted_ && commandQueue) {
            SpawnCommand cmd;
            cmd.category       = SpawnCategory::ExplosionDamage;
            cmd.explosionRect  = getExplosionRect();
            commandQueue->push(cmd);
            damageEmitted_ = true;
            std::cout << "[Boom] ExplosionDamage queued at "
                      << worldStats.position.x << ", " << worldStats.position.y << "\n";
        }

        explosionTimer_ -= dt;
        if (explosionTimer_ <= 0.0f) {
            itemState_ = ItemState::Used;
        }
        return;
    }

    if (!active_) return;

    timer_     -= dt;
    animTimer_ += dt;
    if (animTimer_ >= TOGGLE_INTERVAL) {
        animTimer_   = 0.0f;
        frameToggle_ = !frameToggle_;
    }

    if (timer_ <= 0.0f) {
        exploded_      = true;
        explosionTimer_ = EXPLOSION_SHOW;
        // Stop moving when it explodes
        runtimeStats.velocity = {0.0f, 0.0f};
        std::cout << "[Boom] EXPLOSION!\n";
    }
}

// ─── Rendering ──────────────────────────────────────────────────────────────

void Boom::render(float alpha) {
    if (itemState_ == ItemState::Used) return;
    if (exploded_) {
        // Pivot: center of bottom edge of explosion rect
        Rectangle dest = {
            worldStats.position.x - EXPL_W / 2.0f,
            worldStats.position.y - EXPL_H,
            EXPL_W, EXPL_H
        };
        drawFrameRect("explosion.png", dest);
    } else {
        drawFrame(frameToggle_ ? "bomb_active.png" : "bomb.png");
    }
}

float Boom::getRenderOffsetY() const {
    if (active_ || exploded_) return 0.0f;
    return BaseItem::getRenderOffsetY();
}

// ─── Interaction ─────────────────────────────────────────────────────────────

void Boom::onInteract(Entity& other) {
    if (active_ || exploded_ || itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;

    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getRuntimeStats().storedItemSlot.empty()) {
            p->getRuntimeStatsMutable().storedItemSlot = "Boom";
            itemState_ = ItemState::Used;
            std::cout << "[Boom] Collected by player.\n";
        } else {
            p->setOverlappingItem(this);
        }
    }
}

void Boom::forceInteract(Entity& other) {
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        p->getRuntimeStatsMutable().storedItemSlot = "Boom";
        itemState_ = ItemState::Used;
        std::cout << "[Boom] Swapped by player.\n";
    }
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

Rectangle Boom::getExplosionRect() const {
    // Pivot: center of bottom edge  (worldStats.position.y = bottom of item)
    return {
        worldStats.position.x - EXPL_W / 2.0f,
        worldStats.position.y - EXPL_H,
        EXPL_W,
        EXPL_H
    };
}
