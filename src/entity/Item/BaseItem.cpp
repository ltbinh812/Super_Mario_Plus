#include "BaseItem.h"

static CharacterBaseStats makeBaseStats(float w, float h) {
    CharacterBaseStats bs;
    bs.gravityScale = 0.0f; // Static items: no gravity
    bs.physicsBox = { w, h };
    return bs;
}

static CharacterRuntimeStats makeRuntimeStats(float w, float h) {
    CharacterRuntimeStats rs;
    rs.physicsBox = { w, h };
    rs.velocity = { 0.0f, 0.0f };
    rs.collisionMask = 0xFFFFFFFF; // Dynamic items collide with everything (map limits it to solids)
    return rs;
}

BaseItem::BaseItem(Vector2 worldPos, float frameW, float frameH)
    : Entity(makeBaseStats(frameW, frameH), makeRuntimeStats(frameW, frameH), CharacterWorldStats{}),
      scale_(2.0f), hitW_(frameW), hitH_(frameH)
{
    worldStats.position = worldPos;
    worldStats.startPosition = worldPos;
}

Rectangle BaseItem::getHitbox() const {
    return {
        worldStats.position.x,
        worldStats.position.y - hitH_,
        hitW_,
        hitH_
    };
}

void BaseItem::update(float dt) {
    animTimer_ += dt;
    if (pickupDelay_ > 0.0f) {
        pickupDelay_ -= dt;
    }
}

void BaseItem::drawFrame(const std::string& frameName, Color tint) const {
    auto& reg = ItemAtlasRegistry::getInstance();
    const Texture2D& tex = reg.getTexture(frameName);
    if (tex.id == 0) return;

    Rectangle src = reg.getFrame(frameName);
    // Destination: scale from atlas pixel size to game size
    Rectangle dest = {
        worldStats.position.x,
        worldStats.position.y - hitH_ + getRenderOffsetY(),
        hitW_,
        hitH_
    };
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, tint);
}

void BaseItem::drawFrameRect(const std::string& frameName, Rectangle destRect, Color tint) const {
    auto& reg = ItemAtlasRegistry::getInstance();
    const Texture2D& tex = reg.getTexture(frameName);
    if (tex.id == 0) return;
    Rectangle src = reg.getFrame(frameName);
    destRect.y += getRenderOffsetY();
    DrawTexturePro(tex, src, destRect, { 0, 0 }, 0.0f, tint);
}

void BaseItem::onLand(float floorY) {
    Entity::onLand(floorY);
    runtimeStats.velocity.x = 0.0f;
}

float BaseItem::getRenderOffsetY() const {
    if (runtimeStats.velocity.y == 0.0f && itemState_ != ItemState::Used) {
        return sinf(animTimer_ * 5.0f) * 4.0f;
    }
    return 0.0f;
}
