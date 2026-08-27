#pragma once
#include "Mob.h"
#include <string>

class Boss : public Mob {
private:
    std::string cutsceneId;

public:
    Boss(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config, const std::string& cutsceneId);
    
    void onCutsceneStart(const std::string& triggerId) override;
    void onCutsceneEnd(const std::string& triggerId) override;
    
    void initAnimations(int attackFrames, int runFrames, int idleFrames, int hurtFrames, int dieFrames, int introFrames);
    
    const std::string& getCutsceneId() const { return cutsceneId; }
};
