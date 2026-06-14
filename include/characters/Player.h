#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "PlayerStates.h"

// Note: PlayerState enum is replaced by CharacterState classes.

class Player : public Character {
public:
    Player();
    virtual ~Player();

    virtual void Init(float startX, float startY) override;
    virtual void Draw() override;
    virtual void Update(float dt, float worldWidth) override;

    bool IsCrouching() const { return crouching_; }

private:
    void DrawMarioCharacter(Vector2 center, float scale);

    bool crouching_ = false;
};

#endif
