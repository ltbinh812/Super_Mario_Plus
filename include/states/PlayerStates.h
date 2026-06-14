#ifndef PLAYERSTATES_H
#define PLAYERSTATES_H

#include "CharacterState.h"
#include "Character.h"
#include "raylib.h"
#include <cmath>

class PlayerIdleState : public CharacterState {
public:
    void Enter(Character* character) override {}
    void Update(Character* character, float dt) override;
    void Exit(Character* character) override {}
};

class PlayerRunState : public CharacterState {
public:
    void Enter(Character* character) override {}
    void Update(Character* character, float dt) override;
    void Exit(Character* character) override {}
};

class PlayerJumpState : public CharacterState {
public:
    void Enter(Character* character) override;
    void Update(Character* character, float dt) override;
    void Exit(Character* character) override {}
};

#endif
