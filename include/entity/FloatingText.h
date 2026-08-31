#pragma once
#include <string>
#include "raylib.h"

struct FloatingText {
    std::string text;
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime;
    float maxLifetime;
};
