#include "BaseItem.h"
#include <cmath>

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

// Xem chú thích dài ở BaseItem.h.
void BaseItem::launchAsDrop() {
    baseStats.gravityScale = 160.0f;

    // v0 vừa đủ để đỉnh cao đúng một block trên cạn:  v = sqrt(2 * a * h)
    const float a  = baseStats.gravityScale * 9.8f;
    const float v0 = std::sqrt(2.0f * a * kBlockSize);

    // KHÔNG trôi ngang. Đây chính là thứ đẩy món đồ chui vào tường khi rương
    // nằm trong hốc hẹp; và "rơi ra" thì chỉ cần lên rồi xuống.
    runtimeStats.velocity = { 0.0f, -v0 };

    dropOriginY_ = worldStats.position.y;
    pickupDelay_ = 0.5f;
}

void BaseItem::update(float dt) {
    animTimer_ += dt;
    if (pickupDelay_ > 0.0f) {
        pickupDelay_ -= dt;
    }

    // Chặn cứng độ cao: không bao giờ vượt quá một block so với chỗ bật lên.
    //
    // Chỉ tính v0 là chưa đủ, vì trong nước trọng lực chỉ còn 0.4 lần nên cùng
    // một v0 sẽ đưa món đồ lên cao gấp hai lần rưỡi. Chặn theo VỊ TRÍ THẬT thì
    // đúng trong mọi môi trường, không phụ thuộc gia tốc.
    if (dropOriginY_ >= 0.0f && runtimeStats.velocity.y < 0.0f) {
        if (dropOriginY_ - worldStats.position.y >= kBlockSize) {
            runtimeStats.velocity.y = 0.0f;   // tới trần cho phép, bắt đầu rơi
        }
    }

    if (currentAnim_) {
        currentAnim_->update(dt);
    }
}

void BaseItem::drawFrame(const std::string& frameName, Color tint) const {
    auto& reg = ItemAtlasRegistry::getInstance();
    const Texture2D& tex = reg.getTexture(frameName);
    if (tex.id == 0) return;

    Rectangle src = reg.getFrame(frameName);
    src.x += 0.1f; src.y += 0.1f; src.width -= 0.2f; src.height -= 0.2f;
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
    src.x += 0.1f; src.y += 0.1f; src.width -= 0.2f; src.height -= 0.2f;
    destRect.y += getRenderOffsetY();
    DrawTexturePro(tex, src, destRect, { 0, 0 }, 0.0f, tint);
}

void BaseItem::onLand(float floorY) {
    Entity::onLand(floorY);
    runtimeStats.velocity.x = 0.0f;
}

// Chạm tường ngang — dừng trôi ngang.
// Item drop không được đi ngang (launchAsDrop đã zero-out velocity.x), nhưng
// item Coin/Buff/Key tạo từ map cũ vẫn có thể có velocity.x != 0 nếu constructor
// chưa được làm sạch. Guard thêm ở đây để chắc chắn.
void BaseItem::onHitWall(bool isRightWall, bool isCliff) {
    runtimeStats.velocity.x = 0.0f;
}

// Chạm trần solid trong khi đang bật lên (drop arc) — dừng ngay, bắt đầu rơi.
// Không xử lý: item sẽ tiếp tục đi lên, chui vào tile trần, rồi bị physics
// push ra theo hướng không kiểm soát → nhìn như item "biến mất".
void BaseItem::onHitCeiling(float ceilY) {
    runtimeStats.velocity.y = 0.0f;
    // Không reset dropOriginY_: khi velocity.y = 0 thì gravity sẽ kéo xuống,
    // bộ chặn độ cao trong update() không can thiệp nữa (velocity.y > 0).
}

float BaseItem::getRenderOffsetY() const {
    if (runtimeStats.velocity.y == 0.0f && itemState_ != ItemState::Used) {
        return sinf(animTimer_ * 5.0f) * 4.0f;
    }
    return 0.0f;
}

void BaseItem::setAnimation(ItemState state) {
    auto it = animations_.find(state);
    if (it != animations_.end()) {
        currentAnim_ = &it->second;
    } else {
        currentAnim_ = nullptr;
    }
}

void BaseItem::drawAnim(Color tint) const {
    if (!currentAnim_ || !currentAnim_->isValid()) return;

    const Texture2D& tex = currentAnim_->getTexture();
    if (tex.id == 0) return;

    Rectangle src = currentAnim_->getCurrentSourceRect();
    Rectangle dest = {
        worldStats.position.x,
        worldStats.position.y - hitH_ + getRenderOffsetY(),
        hitW_,
        hitH_
    };
    DrawTexturePro(tex, src, dest, { 0, 0 }, 0.0f, tint);
}

void BaseItem::drawAnimRect(Rectangle destRect, Color tint) const {
    if (!currentAnim_ || !currentAnim_->isValid()) return;

    const Texture2D& tex = currentAnim_->getTexture();
    if (tex.id == 0) return;

    Rectangle src = currentAnim_->getCurrentSourceRect();
    destRect.y += getRenderOffsetY();
    DrawTexturePro(tex, src, destRect, { 0, 0 }, 0.0f, tint);
}

