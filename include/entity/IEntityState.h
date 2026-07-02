#pragma once

template <typename T>
class IEntityState {
public:
    virtual ~IEntityState() = default;
    virtual void onJump(T& entity) {}
    virtual void onMoveRight(T& entity) {}
    virtual void onMoveLeft(T& entity) {}
    virtual void onStopMove(T& entity) {}
    virtual void update(T& entity, float dt) = 0;
    virtual void onEnter(T& entity) = 0;
};
