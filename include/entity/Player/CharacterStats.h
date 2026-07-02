#pragma once
#include "Animation.h"
#include <string>
#include <vector>

struct CharacterStats {
    std::string name;
    int maxHealth;
    int maxMana;
    float moveSpeed;
    float maxSpeed;
    float acceleration;
    float jumpForce;
    float gravityScale;
    std::vector<std::string> skills;
    
    Animation idleAnimation;
    Animation runAnimation;
    Animation jumpAnimation;
};
