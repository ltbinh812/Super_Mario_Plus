#include "PoisonFlask.h"
#include "Entity.h"
#include "Player.h"
#include "CommandQueue.h"
#include "SpawnCommand.h"
#include "Effects.h"
#include "ItemAtlasRegistry.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

static const float BLOCK_SIZE = 32.0f;

PoisonFlask::PoisonFlask(Vector2 worldPos, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = { ((rand() % 200) - 100) * 1.0f, -450.0f };
    
    // Using item_poison_drop first frame as sprite
    animations_[ItemState::Idle] = AtlasAnimation("item_poison_drop", 30, 0.05f, true);
    setAnimation(ItemState::Idle);
}

PoisonFlask::PoisonFlask(Vector2 worldPos, Vector2 initVelocity)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE)
{
    baseStats.gravityScale = 160.0f;
    runtimeStats.velocity = initVelocity;

    animations_[ItemState::Idle] = AtlasAnimation("item_poison_drop", 1, 0.1f);
    setAnimation(ItemState::Idle);
    
    activate();
}

void PoisonFlask::activate() {
    if (active_ || exploded_) return;
    active_ = true;
    timer_  = EXPLODE_TIME;
    std::cout << "[PoisonFlask] Thrown!\n";
}

void PoisonFlask::update(float dt) {
    BaseItem::update(dt);

    if (exploded_) {
        if (!damageEmitted_ && commandQueue) {
            SpawnCommand cmd;
            cmd.category       = SpawnCategory::Entity;
            cmd.type           = EntityType::Explosion;
            cmd.position       = { worldStats.position.x, worldStats.position.y }; 
            cmd.isFacingRight  = true;
            cmd.ownerName      = "PoisonFlask";
            cmd.spawner        = this;
            
            cmd.onHitEffect = [](Entity* target) {
                if (target) {
                    auto poison = std::make_unique<PoisonEffect>();
                    poison->setInPoison(false);
                    target->addEffect(std::move(poison));
                    std::cout << "[PoisonFlask] Poison applied to " << target->getBaseStats().name << "!\n";
                }
            };
            
            commandQueue->push(cmd);
            damageEmitted_ = true;
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
        exploded_ = true;
        runtimeStats.velocity = {0.0f, 0.0f};
        std::cout << "[PoisonFlask] EXPLOSION!\n";
    }
}

void PoisonFlask::render(float alpha) {
    if (itemState_ == ItemState::Used || exploded_) return;
    
    if (!active_) {
        // When not active (just sitting on the ground), render normally (30-frame drop animation)
        drawAnim();
        return;
    }
    
    auto& reg = ItemAtlasRegistry::getInstance();
    Rectangle src = reg.getFrame("item_poison_drop");
    src.width = 32.0f; // Only first frame
    Texture2D tex = reg.getTexture("item_poison_drop");
    
    if (tex.id == 0) return;
    
    Color tint = WHITE;
    if (active_) {
        tint = frameToggle_ ? Color{255, 255, 255, 64} : WHITE;
    }
    
    float drawW = hitW_;
    float drawH = hitH_;
    
    Rectangle dest = {
        worldStats.position.x - drawW / 2.0f,
        worldStats.position.y - drawH + getRenderOffsetY(),
        drawW,
        drawH
    };
    
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, tint);
}

float PoisonFlask::getRenderOffsetY() const {
    if (active_ || exploded_) return 0.0f;
    return BaseItem::getRenderOffsetY();
}

void PoisonFlask::onInteract(Entity& other) {
    if (active_ || exploded_ || itemState_ == ItemState::Used) return;
    if (pickupDelay_ > 0.0f) return;

    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        if (p->getRuntimeStats().storedItemSlot.empty()) {
            p->getRuntimeStatsMutable().storedItemSlot = "Poison";
            itemState_ = ItemState::Used;
            std::cout << "[PoisonFlask] Collected by player.\n";
        } else {
            p->setOverlappingItem(this);
        }
    }
}

void PoisonFlask::forceInteract(Entity& other) {
    Player* p = dynamic_cast<Player*>(&other);
    if (p) {
        p->getRuntimeStatsMutable().storedItemSlot = "Poison";
        itemState_ = ItemState::Used;
        std::cout << "[PoisonFlask] Swapped by player.\n";
    }
}
