#pragma once
#include <string>
class Player;

class ISkill {
protected:
    float manaCost = 0.0f;
    float duration = 0.0f;
    std::string animationName;
public: 
    virtual ~ISkill() = default;
    virtual void execute(Player& player) = 0;

    float getManaCost() const { return manaCost; }
    float getDuration() const { return duration; }
    const std::string& getAnimationName() const { return animationName; }
};
    