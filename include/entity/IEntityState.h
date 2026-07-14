#pragma once

template <typename T>
class IEntityState {
public:
    virtual ~IEntityState() = default;

    virtual void update(float dt) = 0;
    virtual void onEnter() = 0;
    virtual void onExit() = 0;
};
