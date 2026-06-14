#ifndef FIREBALLABILITY_H
#define FIREBALLABILITY_H

#include "AbilityStrategy.h"
#include "Character.h"
#include <iostream>

class FireballAbility : public AbilityStrategy {
public:
    void Execute(Character* character) override {
        // Just print or spawn a fireball later
        // Currently it just gives a small impulse to demonstrate
        if (character) {
            character->SetVelocityX(character->GetVelocityX() * 1.5f); 
            // In a real implementation, we would spawn an Entity
        }
    }
};

#endif
