#pragma once
#include "ITransition.h"
#include <raylib.h>

class IrisTransition : public ITransition {
private:
    float maxRadius;
    float currentRadius;
    float speedOut;
    float speedIn;
    float featherSize;
    
    bool isTransitioningOut;
    bool isFinished;
    Vector2 center;
    Color overlayColor;

public:
    // speedOut: radius units per second when closing. Default 1600.0f
    // speedIn: radius units per second when opening. Default 800.0f
    // color: Default BLACK
    IrisTransition(float speedOut = 1600.0f, float speedIn = 800.0f, Color color = BLACK);
    ~IrisTransition() override = default;

    void Start(bool isTransitioningOut) override;
    void Update(float dt) override;
    void Render() const override;
    bool IsFinished() const override;
};
