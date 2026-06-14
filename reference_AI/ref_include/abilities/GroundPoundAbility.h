#ifndef GROUNDPOUNDABILITY_H
#define GROUNDPOUNDABILITY_H

#include "AbilityStrategy.h"
#include "Character.h"

class GroundPoundAbility : public AbilityStrategy {
public:
    void Execute(Character* character) override {
        if (character && !character->IsOnGround()) {
            // Slam downward quickly
            character->SetVelocityY(1500.0f);
        }
    }
};

#endif
