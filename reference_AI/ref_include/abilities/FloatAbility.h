#ifndef FLOATABILITY_H
#define FLOATABILITY_H

#include "AbilityStrategy.h"
#include "Character.h"

class FloatAbility : public AbilityStrategy {
public:
    void Execute(Character* character) override {
        if (character && !character->IsOnGround() && character->GetVelocityY() > 0.0f) {
            // Slow down falling
            character->SetVelocityY(50.0f);
        }
    }
};

#endif
