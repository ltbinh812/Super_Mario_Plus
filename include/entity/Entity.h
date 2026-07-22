#pragma once 
#include "raylib.h"
#include "IEntityState.h"
#include "CharacterStats.h"

class Entity {
    protected:
    CharacterBaseStats baseStats;
    CharacterRuntimeStats runtimeStats;
    CharacterWorldStats worldStats;

    public: 
        Entity(CharacterBaseStats &bS, CharacterRuntimeStats &rS, CharacterWorldStats &wS);
        virtual ~Entity() = default;
        virtual void update(float dt) = 0;
        virtual void render(float alpha) = 0;

        // Physics helpers (overridable by subclasses)
        virtual void applyGravity(float dt) {}
        virtual void updatePosition(float dt) {}
        virtual void checkGroundCollision(float groundY) {}
        virtual void updateStateFromPhysics() {}

        const CharacterBaseStats& getBaseStats() const { return baseStats; }
        const CharacterRuntimeStats& getRuntimeStats() const { return runtimeStats; }
        const CharacterWorldStats& getWorldStats() const { return worldStats; }
    
};  