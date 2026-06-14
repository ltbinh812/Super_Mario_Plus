#ifndef DASHABILITY_H
#define DASHABILITY_H

#include "AbilityStrategy.h"
#include "Character.h"

class DashAbility : public AbilityStrategy {
public:
    void Execute(Character* character) override {
        if (character) {
            float dashForce = 1200.0f; // Dash force multiplier
            if (character->IsFacingRight()) {
                character->SetVelocityX(dashForce);
            } else {
                character->SetVelocityX(-dashForce);
            }
        }
    }
};

#endif
