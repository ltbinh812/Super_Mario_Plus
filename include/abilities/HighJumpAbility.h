#ifndef HIGHJUMPABILITY_H
#define HIGHJUMPABILITY_H

#include "AbilityStrategy.h"
#include "Character.h"

class HighJumpAbility : public AbilityStrategy {
public:
    void Execute(Character* character) override {
        if (character && character->IsOnGround()) {
            character->SetVelocityY(character->GetJumpForce() * 1.5f); // 50% higher jump
            character->SetOnGround(false);
        }
    }
};

#endif
