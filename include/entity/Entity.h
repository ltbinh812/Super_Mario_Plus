#pragma once 
#include "raylib.h"
#include "Animation.h"
#include <memory>

class Entity {
    protected:
    Animation* currentAnimation;
    Vector2 prevPosition;
    Vector2 position;
    Vector2 boxSize;
    Vector2 velocity = {0, 0};
    Vector2 acceleration = {0, 100};

    public:
    virtual ~Entity() = default;
    Entity(Vector2 pos, Vector2 boxsize);
    virtual void handleInput() {};
    virtual void process() {};
    virtual void update(float dt) = 0;
    virtual void render(float alpha) const = 0;

    virtual void jump() {}
    virtual void moveRight() {}
    virtual void moveLeft() {}
    virtual void stopMove() {}

    virtual void addForce(Vector2 force);
};  