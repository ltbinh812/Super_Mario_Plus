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

        CharacterBaseStats& getBaseStats() { return baseStats; }
        CharacterRuntimeStats& getRuntimeStats() { return runtimeStats; }
        CharacterWorldStats& getWorldStats() { return worldStats; }
    
};  