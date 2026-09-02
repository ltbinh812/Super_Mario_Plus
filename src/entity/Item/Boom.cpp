#include "Boom.h"
#include "Entity.h"
#include "Player.h"
#include "CommandQueue.h"
#include "SpawnCommand.h"
#include "Effects.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "infrastructure/AssetManager.h"

static const float BLOCK_SIZE = 32.0f;

// ─── Constructors ───────────────────────────────────────────────────────────

Boom::Boom(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    // Pop up when spawned from a chest/luckyblock
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };

    animations_[ItemState::Idle] = AtlasAnimation("bomb_anim", 10, 0.1f);
    setAnimation(ItemState::Idle);
}

// Pre-activated thrown bomb: starts counting down immediately
Boom::Boom(Vector2 worldPos, Vector2 initVelocity)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = initVelocity;

    animations_[ItemState::Idle] = AtlasAnimation("bomb_anim", 10, 0.1f);
    setAnimation(ItemState::Idle);
    
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
        // Push Explosion command exactly once so CombatSystem handles it
        if (!damageEmitted_ && commandQueue) {
            SpawnCommand cmd;
            cmd.category       = SpawnCategory::Entity;
            cmd.type           = EntityType::Explosion;
            cmd.position       = { worldStats.position.x, worldStats.position.y }; // Center
            cmd.isFacingRight  = true;
            cmd.ownerName      = "Boom";
            cmd.spawner        = this;
            
            cmd.onHitEffect = [](Entity* target) {
                if (target) {
                    auto burn = std::make_unique<LavaEffect>();
                    burn->setInLava(false);
                    target->addEffect(std::move(burn));
                    std::cout << "[Explosion] Damage and Burn applied to " << target->getBaseStats().name << "!\n";
                }
            };
            
            commandQueue->push(cmd);
            damageEmitted_ = true;
            std::cout << "[Boom] Explosion Entity queued at "
                      << worldStats.position.x << ", " << worldStats.position.y << "\n";
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
        // Stop moving when it explodes
        runtimeStats.velocity = {0.0f, 0.0f};
        std::cout << "[Boom] EXPLOSION!\n";
        PlaySound(AssetManager::getInstance().getSound("boom_explosion_sound"));
    }
}

// ─── Rendering ──────────────────────────────────────────────────────────────

void Boom::render(float alpha) {
    if (itemState_ == ItemState::Used || exploded_) return;
    if (active_) {
        Color tint = frameToggle_ ? Color{255, 255, 255, 64} : WHITE;
        if (currentAnim_ && currentAnim_->isValid()) {
            drawAnim(tint);
        } else {
            drawFrame("bomb.png", tint);
        }
    } else {
        if (currentAnim_ && currentAnim_->isValid()) {
            drawAnim();
        } else {
            drawFrame("bomb.png");
        }
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
            PlaySound(AssetManager::getInstance().getSound("pickup_sound"));
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
        PlaySound(AssetManager::getInstance().getSound("pickup_sound"));
    }
}


