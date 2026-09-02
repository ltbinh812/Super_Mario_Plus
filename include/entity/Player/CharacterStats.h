#pragma once
#include "Animation.h"
#include <string>
#include <cstdint>
#include <vector>
#include "raylib.h"
#include "TileMap.h"
#include "FloatingText.h"

struct CharacterBaseStats {
    std::string name = "Default";

    int maxHealth = 100;
    int maxMana = 100;
    int maxBreath = 100;

    float moveVelocity = 100.0f;
    float jumpVelocity = 100.0f;

    float gravityScale = 10.0f;  
    
    Vector2 physicsBox = {0.0f, 0.0f}; 
    Vector2 crouchBox = {0.0f, 0.0f};
    bool avoidCliffsAndWater = false;
};


struct CharacterRuntimeStats {
    int health = 100;
    int mana   = 0;
    int breath = 100;
    float manaAccumulator = 0.0f;
    float breathAccumulator = 0.0f;
    float drownDamageTimer = 0.0f;

    Vector2 physicsBox = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f}; 

    bool isGrounded = false;
    bool isOverlappingLadder = false;
    uint32_t collisionMask = 0xFFFFFFFF;
    float ignoreOneWayTimer = 0.0f;
    float ignoreLadderTimer = 0.0f;
    float iframeTimer = 0.0f;
    float disableInputTimer = 0.0f;

    CollisionType currentLiquid = CollisionType::None;
    bool isPartiallyOutsideLiquid = true;

    // Inventory slots
    std::string storedItemSlot = ""; // Can hold "Boom", "Speed", "Shield", etc.
};

struct PartyInventory {
    int coins = 0;
    int keys = 0;
};


struct CharacterWorldStats {
    Animation* animation = nullptr;
    bool isFacingRight = true;

    Vector2 previousPos = {0.0f, 0.0f};
    Vector2 position = {0.0f, 0.0f};
    Vector2 startPosition = {0.0f, 0.0f};
    std::vector<FloatingText> floatingTexts;
};

