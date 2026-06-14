#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

class Character;

class CharacterState {
public:
    virtual ~CharacterState() = default;

    virtual void Enter(Character* character) = 0;
    virtual void Update(Character* character, float dt) = 0;
    virtual void Exit(Character* character) = 0;
};

#endif
