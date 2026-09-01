#pragma once
#include "Mob.h"
#include <string>

class Boss : public Mob {
private:
    std::string cutsceneId;
    bool isDebugMode = false;
    bool isWaitingForCutscene = false;

    // Boss đã dùng cú dịch chuyển vào trận chưa (chỉ một lần cả màn).
    bool hasTeleportedOnSight_ = false;

    // Kiểm tra người chơi có vào bán kính kFirstSightBlocks chưa; nếu có thì
    // nhấp nháy tới cạnh họ. Xem chú thích đầy đủ trong Boss.cpp.
    void updateFirstSightTeleport();

public:
    Boss(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config, const std::string& cutsceneId);
    
    void update(float dt) override;
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;
    
    void onCutsceneStart(const std::string& triggerId) override;
    void onCutsceneEnd(const std::string& triggerId) override;
    
    void initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames, int introFrames);
    
    const std::string& getCutsceneId() const { return cutsceneId; }
    bool getIsWaitingForCutscene() const { return isWaitingForCutscene; }

    // Bán kính "lần đầu nhìn thấy người chơi", tính bằng số block.
    static constexpr float kFirstSightBlocks = 5.0f;

    // Bật/tắt chế độ điều khiển boss bằng tay để chỉnh timing hitbox.
    // Trước đây gắn cứng vào phím P bên trong update() — xem Boss::update().
    void enterDebugMode(bool enable);
};
