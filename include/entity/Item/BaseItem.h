#pragma once
#include "Entity.h"
#include "ItemAtlasRegistry.h"
#include "AtlasAnimation.h"
#include "raylib.h"
#include <string>
#include <unordered_map>

// State Pattern: item lifecycle
enum class ItemState { Idle, Active, Used };

// Abstract base for all static interactive items.
// - No gravity applied (static physics)
// - Provides drawFrame() helper using the Flyweight ItemAtlasRegistry
// - Override onInteract() for pickup/collision logic
class BaseItem : public Entity {
public:
    // worldPos: position in GAME pixels (after LDtk scale)
    // frameW/frameH: collision hitbox size in game pixels
    BaseItem(Vector2 worldPos, float frameW, float frameH);
    virtual ~BaseItem() = default;

    virtual void update(float dt) override;
    virtual void render(float alpha) override = 0;

    virtual void onHitWall(bool isRightWall) override {}
    virtual void onHitCeiling(float ceilY) override {}
    virtual void onLand(float floorY) override;

    // Allows items to have dynamic rendering offsets (e.g. hovering)
    virtual float getRenderOffsetY() const;

    // Override to handle player contact
    virtual void onInteract(Entity& other) {}
    
    // Default interaction behavior when swapped
    virtual void forceInteract(Entity& other) { onInteract(other); }

    Rectangle getHitbox() const override;

    bool getIsActive() const override { return itemState_ != ItemState::Used; }

    // For state persistence across level transitions
    ItemState getItemState() const { return itemState_; }
    void setItemState(ItemState s) { itemState_ = s; }

    const std::string& getIid() const { return iid_; }
    void setIid(const std::string& iid) { iid_ = iid; }

    // Delay before this item can be picked up (e.g. when spawned from a chest)
    void setPickupDelay(float delay) { pickupDelay_ = delay; }
    float getPickupDelay() const { return pickupDelay_; }

    // Dynamic solid physics (returns {0,0,0,0} if not solid)
    virtual Rectangle getSolidRect() const { return {0, 0, 0, 0}; }

protected:
    ItemState itemState_ = ItemState::Idle;
    std::string iid_ = "";
    float scale_;   // 2.0f (LDtk 16px -> game 32px)
    float animTimer_ = 0.0f;
    float pickupDelay_ = 0.0f;

    std::unordered_map<ItemState, AtlasAnimation> animations_;
    AtlasAnimation* currentAnim_ = nullptr;

    // Set the current animation based on the state
    void setAnimation(ItemState state);

    // Draw the current frame of the current animation
    void drawAnim(Color tint = WHITE) const;
    void drawAnimRect(Rectangle destRect, Color tint = WHITE) const;

    // Draw a single frame from the atlas at this item's world position
    void drawFrame(const std::string& frameName, Color tint = WHITE) const;

    // Draw a frame stretched to cover given rect in world space
    void drawFrameRect(const std::string& frameName, Rectangle destRect, Color tint = WHITE) const;

    float hitW_;
    float hitH_;
};
