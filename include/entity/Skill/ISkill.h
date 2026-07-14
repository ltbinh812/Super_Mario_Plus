#pragma once
#include <string>
class Player;

class ISkill {
protected:
    float manaCost;
    std::string animationName;
public: 
    virtual ~ISkill() = default;
    virtual void execute(Player& player) = 0;

};
    