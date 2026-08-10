#pragma once

template <typename T>
class IEntityState {
public:
    virtual ~IEntityState() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void update(float dt) = 0;

    // Exit guard — override to lock state (e.g. during skill animation)
    virtual bool canExit() const { return true; }

    // Input event hooks — default no-op so subclasses only override what they need
    virtual void onMoveLeft() {}
    virtual void onMoveRight() {}
    virtual void onJump() {}
    virtual void onCrouch() {}
    virtual void onAttack() {}
    virtual void onStopLeft() {}
    virtual void onStopRight() {}
    virtual void onStopCrouch() {}
    virtual void onClimb() {}
};
