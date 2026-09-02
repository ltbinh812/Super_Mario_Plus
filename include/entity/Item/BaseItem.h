#pragma once
#include "Entity.h"
#include "ItemAtlasRegistry.h"
#include "AtlasAnimation.h"
#include "ItemState.h"
#include "raylib.h"
#include <string>
#include <unordered_map>

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
    virtual void process(const std::vector<class Player*>& players) {}
    virtual void render(float alpha) override = 0;

    virtual void onHitWall(bool isRightWall, bool isCliff = false) override {}
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

    // =========================================================================
    // Bật vật phẩm lên rồi để nó rơi xuống — dùng cho đồ RƠI RA (rương mở, quái
    // chết), không dùng cho đồ ném.
    //
    // LỖI ĐÃ SỬA — một số rương "không rơi ra gì cả".
    // Coin/Key/Buff vốn tự bật lên với v0 = 450 và trôi ngang tới ±100/giây.
    // Với gia tốc 160*9.8 thì đỉnh cao tới 65 đơn vị = ĐÚNG 2 BLOCK; còn dưới
    // nước trọng lực chỉ còn 0.4 lần nên đỉnh vọt lên 161 đơn vị = 5 BLOCK.
    // Rương kê trong hốc hẹp hay đặt dưới nước thì món đồ bắn thẳng vào trần
    // hoặc trôi ngang ra khỏi tầm mắt — nhìn ra ngoài y như rương rỗng.
    //
    // Nay bật lên tối đa ĐÚNG MỘT BLOCK và không trôi ngang.
    // =========================================================================
    void launchAsDrop();

    // Một block trong hệ toạ độ thế giới (tile 16px * hệ số 2).
    static constexpr float kBlockSize = 32.0f;

    // Dynamic solid physics (returns {0,0,0,0} if not solid)
    virtual Rectangle getSolidRect() const { return {0, 0, 0, 0}; }

protected:
    ItemState itemState_ = ItemState::Idle;
    // Mốc độ cao lúc bắt đầu bật lên; -1 = không phải đồ rơi ra.
    float dropOriginY_ = -1.0f;
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
