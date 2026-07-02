#pragma once
#include <string>
class Player;
class ISkill {
protected:
    std::string skillName;
    float cooldownTime;
    float currentCooldown;
    float manaCost;
public: 
    virtual ~ISkill() = default;
    virtual void execute(Player& player) = 0;
    virtual void update(float dt) {
        if (currentCooldown > 0) {
            currentCooldown -= dt;
            if (currentCooldown < 0) {
                currentCooldown = 0;
            }
        }
    }
    bool isReady() const {
        return currentCooldown <= 0;
    }
    void startCooldown() {
        currentCooldown = cooldownTime;
    }
};