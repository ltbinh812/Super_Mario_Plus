#ifndef GOOMBA_H
#define GOOMBA_H

#include "Character.h"

class Goomba : public Character {
public:
    Goomba();
    virtual ~Goomba() = default;

    void Init(float startX, float startY) override;
    void Update(float dt, float worldWidth) override;
    void Draw() override;
    void ResolveCollision(Rectangle platRect) override;

private:
    float moveDirection_ = -1.0f; // -1 for left, 1 for right
    float patrolSpeed_ = 100.0f;
};

#endif
