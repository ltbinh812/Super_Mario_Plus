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

    // Combat data (loaded from JSON by PlayerFactory)
    int attackPower = 0;
    int defensePower = 0;
    Rectangle hitboxConfig = {0, 0, 0, 0};  // {offsetX, offsetY, w, h}
public: 
    ISkill(float mn = 0.0f, float dr = 0.0f) : manaCost(mn), duration(dr) {}
    virtual ~ISkill() = default;
    virtual void execute(Player& player) = 0;

    float getManaCost() const { return manaCost; }
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

    // Combat data getters
    int getAttackPower() const { return attackPower; }
    int getDefensePower() const { return defensePower; }
    Rectangle getHitboxConfig() const { return hitboxConfig; }
    void setCombatData(int atk, int def, Rectangle box) {
        attackPower = atk;
        defensePower = def;
        hitboxConfig = box;
    }
};
    