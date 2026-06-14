#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

class Entity {
public:
    virtual ~Entity() = default;

    virtual void Update(float dt, float worldWidth) = 0;
    virtual void Draw() = 0;
    virtual void ResolveCollision(Rectangle otherBounds) = 0;

    virtual Rectangle GetBounds() const = 0;
    
    virtual Vector2 GetPosition() const { return pos_; }
    virtual void SetPosition(Vector2 pos) { pos_ = pos; }

protected:
    Vector2 pos_ = {0.0f, 0.0f};
    Vector2 vel_ = {0.0f, 0.0f};
    float width_ = 0.0f;
    float height_ = 0.0f;
};

#endif
