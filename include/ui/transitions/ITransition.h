#pragma once

class ITransition {
public:
    virtual ~ITransition() = default;

    // isTransitioningOut = true: Transition Out (e.g., shrinking)
    // isTransitioningOut = false: Transition In (e.g., expanding)
    virtual void Start(bool isTransitioningOut) = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() const = 0;
    virtual bool IsFinished() const = 0;
};
