#pragma once
#include <string>
#include "raylib.h"
class Player;

class ISkill {
protected:
    float manaCost;
    float duration;
    std::string animationName;
    std::string nextComboSkillName = "";  // Name of the next skill in the combo chain

    float hitboxStartTime = 0.0f;
    float hitboxEndTime = 0.0f;

    // Pacing data (loaded from JSON by PlayerFactory)
    float recoveryDuration = 0.1f;       // pause after animation before next action
    float hitStopDuration = 0.05f;        // freeze on hit confirmation
    float anticipationDuration = 0.0f;    // wind-up before animation starts (0 for normal attacks)

    // Combat data (loaded from JSON by PlayerFactory)
    int attackPower = 0;
    int defensePower = 0;
    Rectangle hitboxConfig = {0, 0, 0, 0};  // {offsetX, offsetY, w, h}
    
    // Movement multiplier during skill execution
    float moveControl = 0.0f;
    float dashMultiplier = 2.0f;
public: 
    ISkill(float mn = 0.0f, float dr = 0.0f) : manaCost(mn), duration(dr) {}
    virtual ~ISkill() = default;
    virtual void execute(Player& player) = 0;

    float getManaCost() const { return manaCost; }
    void setManaCost(float mn) { manaCost = mn; }
    float getDuration() const { return duration; }
    const std::string& getAnimationName() const { return animationName; }
    const std::string& getNextComboSkillName() const { return nextComboSkillName; }
    bool hasNextCombo() const { return !nextComboSkillName.empty(); }

    void setDurationAndHitbox(float dur, float hitStart, float hitEnd) {
        duration = dur;
        hitboxStartTime = hitStart;
        hitboxEndTime = hitEnd;
    }

    float getHitboxStartTime() const { return hitboxStartTime; }
    float getHitboxEndTime() const { return hitboxEndTime; }

    // Pacing getters/setters
    float getRecoveryDuration() const { return recoveryDuration; }
    float getHitStopDuration() const { return hitStopDuration; }
    float getAnticipationDuration() const { return anticipationDuration; }
    void setPacingData(float recovery, float hitstop, float anticipation) {
        recoveryDuration = recovery;
        hitStopDuration = hitstop;
        anticipationDuration = anticipation;
    }

    // Combat data getters
    int getAttackPower() const { return attackPower; }
    int getDefensePower() const { return defensePower; }
    Rectangle getHitboxConfig() const { return hitboxConfig; }
    void setCombatData(int atk, int def, Rectangle box) {
        attackPower = atk;
        defensePower = def;
        hitboxConfig = box;
    }

    float getMoveControl() const { return moveControl; }
    void setMoveControl(float mc) { moveControl = mc; }
    
    float getDashMultiplier() const { return dashMultiplier; }
    void setDashMultiplier(float dm) { dashMultiplier = dm; }
};
    