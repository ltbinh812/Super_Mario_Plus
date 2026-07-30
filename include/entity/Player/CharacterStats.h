#pragma once
#include "Animation.h"
#include <string>
#include <cstdint>
#include "raylib.h"

struct CharacterBaseStats {
    std::string name = "Default";

    int maxHealth = 100;
    int maxMana = 100;

    float moveVelocity = 100.0f;
    float jumpVelocity = 100.0f;

    float gravityScale = 10.0f;  
    
    Vector2 hitbox = {0.0f, 0.0f};
};


struct CharacterRuntimeStats {
    int health = 100;
    int mana   = 0;

    int attack = 0;
    int defense = 0;

    Vector2 hitbox = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f}; 

    bool isGrounded = false;
    bool isOverlappingLadder = false;
    uint32_t collisionMask = 0xFFFFFFFF;
    float ignoreOneWayTimer = 0.0f;
    float ignoreLadderTimer = 0.0f;
};


struct CharacterWorldStats {
    Animation* animation = nullptr;
    bool isFacingRight = true;

    Vector2 position = {0.0f, 0.0f};
};

