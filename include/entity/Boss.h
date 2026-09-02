#pragma once
#include "Mob.h"
#include <string>

class Boss : public Mob {
private:
    std::string cutsceneId;
    bool isDebugMode = false;
    bool isWaitingForCutscene = false;

    // Đã dùng cú dịch chuyển RA MẮT (lần đầu, kích hoạt bằng tầm nhìn) chưa.
    bool hasFirstTeleport_ = false;

    // Số giây kể từ lần cuối boss ăn đòn CỦA NGƯỜI CHƠI.
    // Sát thương môi trường (lava, độc) KHÔNG đặt lại đồng hồ này — xem
    // Boss::onDamagedBy.
    float noDamageTimer_ = 0.0f;

    // Ngưỡng chờ cho lần dịch chuyển kế tiếp, bốc ngẫu nhiên trong
    // [kStallMin, kStallMax] sau mỗi lần nhảy.
    float nextStallDelay_ = kStallMin;

    void updateTeleport(float dt);

    // Bốc ngưỡng chờ mới trong [kStallMin, kStallMax] và reset đồng hồ.
    void armNextStall();

public:
    Boss(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config, const std::string& cutsceneId);
    
    void update(float dt) override;
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;
    void onDamagedBy(Entity* attacker, int amount) override;
    
    void onCutsceneStart(const std::string& triggerId) override;
    void onCutsceneEnd(const std::string& triggerId) override;
    
    void initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames, int introFrames);
    
    const std::string& getCutsceneId() const { return cutsceneId; }
    bool getIsWaitingForCutscene() const { return isWaitingForCutscene; }

    // Lần dịch chuyển ĐẦU: người chơi phải lọt vào bán kính này (tính bằng block).
    static constexpr float kFirstSightBlocks = 5.0f;

    // Những lần SAU: phải trôi qua ngần này giây mà boss không ăn đòn nào của
    // người chơi. Bốc ngẫu nhiên trong khoảng để không thành nhịp máy đếm.
    static constexpr float kStallMin = 5.0f;
    static constexpr float kStallMax = 10.0f;

    // Bật/tắt chế độ điều khiển boss bằng tay để chỉnh timing hitbox.
    // Trước đây gắn cứng vào phím P bên trong update() — xem Boss::update().
    void enterDebugMode(bool enable);
};
