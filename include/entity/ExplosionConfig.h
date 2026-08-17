#pragma once
#include <string>

struct ExplosionConfig {
    float lifetime       = 0.5f;
    int   damage         = 20;
    float hitboxW        = 100.0f;
    float hitboxH        = 100.0f;
    float offsetX        = 50.0f;   // horizontal offset from player (flips with facing)
    float offsetY        = 0.0f;    // vertical offset from player (negative = above)
    std::string textureName = "";
    int   frameNum       = 1;
    float frameTime      = 0.1f;
};
