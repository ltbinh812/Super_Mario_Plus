#pragma once
#include "Entity.h"
#include "FireballConfig.h"
#include "Animation.h"
#include <memory>
#include <string>
#include <vector>

class Fireball : public Entity {
private:
    bool isActive = true;
    float lifetime;
    int attackPower;
    float curveAmplitude;
    float curveFrequency;
    float elapsedTime = 0.0f;
    float originY;
    bool  beamFromOwner = false;         // xem FireballConfig
    bool  alignFramesByContent = false;
    std::string textureName;             // khoá cache canh khung theo nội dung
    std::string soundKey;                // âm thanh đang phát (minh1: sfx)
    // Tâm nội dung từng khung, tính SẴN lúc dựng đạn (xem Fireball.cpp).
    // nullptr khi không bật alignFramesByContent.
    const std::vector<Vector2>* frameCenters_ = nullptr;
    std::unique_ptr<Animation> animation;
    Entity* spawner;
    float hitboxOffsetX = 0.0f;
    float hitboxOffsetY = 0.0f;
    
public:
    // We pass initial position, facing direction, and per-character config
    Fireball(Vector2 startPos, bool isFacingRight, const FireballConfig& config, Entity* spawner);
    ~Fireball() override;

    void update(float dt) override;
    void render(float alpha) override;

    // Vẽ tia sáng nối từ người bắn tới quả đạn (chưởng kamehameha).
    void renderOwnerBeam() const;

    // Tắt tiếng đang phát của viên đạn. Gọi khi đạn biến mất, bằng mọi đường:
    // hết thời gian, chạm tường, trúng mục tiêu, hoặc bị huỷ cùng màn chơi.
    void stopSound();

    void onHitWall(bool isRightWall, bool isCliff = false) override;
    void onCollide(Entity& other) override;

    // Combat interface
    bool hasActiveHitbox() const override;
    Hitbox getActiveHitbox() override;
    Rectangle getOffsetHitbox() const;
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;

    bool getIsActive() const override { return isActive; }
    // Không còn là hàm inline: giờ phải tắt cả tiếng, nên phần thân nằm ở .cpp.
    void deactivate() override;
};
