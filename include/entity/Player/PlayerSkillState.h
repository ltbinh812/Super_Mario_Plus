#pragma once
#include "PlayerState.h"
#include "ISkill.h"

class PlayerSkillState : public PlayerState {
    private:
    ISkill* currentSkill = nullptr;
    ISkill* nextSkill = nullptr;  // Queued combo skill
    float timer = 0.0f;
    bool hasExecuted = false;
    // Chiêu endOnLanding phải cất cánh trước đã: nếu người chơi bấm ngay lúc
    // còn chạm đất, không được kết thúc chiêu ở chính khung hình đầu tiên.
    bool hasLeftGround = false;
    // Đã ghìm thời gian còn lại khi tiếp đất chưa (chỉ làm một lần mỗi chiêu).
    bool hasCutOnLanding = false;

    public:

    PlayerSkillState(Player& player);
    
    void setSkill(ISkill* skill) { currentSkill = skill; }
    void resetTimer() { if (currentSkill) timer = currentSkill->getDuration(); }

    void onEnter() override;
    void onExit() override;
    void update(float dt) override;
    bool canExit() const override { return timer <= 0.0f; }

    void onAttack() override;

    void onMoveLeft() override;
    void onMoveRight() override;
    void onStopLeft() override;
    void onStopRight() override;

    void forceStop() { timer = 0.0f; }

    const ISkill* getCurrentSkill() const { return currentSkill; }
    float getElapsedTime() const;
    bool isHitboxActive() const;
};
