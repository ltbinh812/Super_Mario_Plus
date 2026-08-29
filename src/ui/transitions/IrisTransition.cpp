#include "IrisTransition.h"
#include <algorithm>
#include <cmath>

IrisTransition::IrisTransition(float speedOutVal, float speedInVal, Color color)
    : speedOut(speedOutVal), speedIn(speedInVal), featherSize(250.0f), isTransitioningOut(false), isFinished(true), overlayColor(color) {
    
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    center = { screenW / 2.0f, screenH / 2.0f };

    // Calculate maximum radius to cover the entire screen (diagonal from center)
    maxRadius = std::sqrt(center.x * center.x + center.y * center.y) + 10.0f; // Add a small buffer
    currentRadius = maxRadius;
}

void IrisTransition::Start(bool transitioningOut) {
    isTransitioningOut = transitioningOut;
    isFinished = false;

    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    center = { screenW / 2.0f, screenH / 2.0f };
    maxRadius = std::sqrt(center.x * center.x + center.y * center.y) + 10.0f;

    if (isTransitioningOut) {
        currentRadius = maxRadius; // Start fully open
    } else {
        currentRadius = -featherSize; // Start fully closed, sinking the entire feather zone
    }
}

void IrisTransition::Update(float dt) {
    if (isFinished) return;

    if (isTransitioningOut) {
        // Shrink the circle (close)
        currentRadius -= speedOut * dt;
        if (currentRadius <= -featherSize) {
            currentRadius = -featherSize;
            isFinished = true;
        }
    } else {
        // Expand the circle (open)
        currentRadius += speedIn * dt;
        if (currentRadius >= maxRadius) {
            currentRadius = maxRadius;
            isFinished = true;
        }
    }
}

void IrisTransition::Render() const {
    if (isFinished && !isTransitioningOut) return; // If Iris In is finished, no overlay needed
    if (isFinished && isTransitioningOut) {
        // If Iris Out is finished, the screen is entirely black
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), overlayColor);
        return;
    }

    // Use DrawRing to draw a black mask around the transparent circle.
    // The inner radius is the currentRadius.
    // The outer radius must be large enough to cover the rest of the screen.
    // Wait, DrawRing only draws up to outerRadius. If currentRadius is very small, 
    // outerRadius needs to cover the corners. Let's make outerRadius sufficiently large.
    
    // outerRadius can just be maxRadius + something large. Actually, maxRadius * 2 is safe enough.
    float outerRadius = maxRadius * 2.0f; 
    
    // Draw feather (gradient edge) using Linear Alpha Bands to prevent accumulative blending bug
    int steps = 25;             // Number of steps for the gradient
    float stepSize = featherSize / (float)steps;

    // Draw accumulative rings to create a smooth gradient without overlapping blending artifacts
    for (int i = 0; i < steps; ++i) {
        float r1 = currentRadius + i * stepSize;
        float r2 = r1 + stepSize;
        
        if (r2 <= 0.0f) continue;       // Skip rings that are completely shrunk into the center
        if (r1 < 0.0f) r1 = 0.0f;       // Prevent raylib from drawing inverted radii
        
        // Linear alpha from 0 (inner edge) to 255 (outer edge)
        float progress = (float)i / (float)(steps - 1);
        unsigned char alpha = (unsigned char)(progress * 255.0f);
        
        Color ringColor = overlayColor;
        ringColor.a = alpha;
        
        // Add a small overlap (+1.0f) to outer radius to prevent 1px visual gaps
        DrawRing(center, r1, r2 + 1.0f, 0.0f, 360.0f, 64, ringColor);
    }
    
    // Draw the solid outer part
    float endR = currentRadius + featherSize;
    if (endR < 0.0f) endR = 0.0f;
    if (endR < outerRadius) {
        DrawRing(center, endR, outerRadius, 0.0f, 360.0f, 64, overlayColor);
    }
}

bool IrisTransition::IsFinished() const {
    return isFinished;
}
