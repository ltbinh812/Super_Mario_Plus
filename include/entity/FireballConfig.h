#pragma once
#include <string>

struct FireballConfig {
    float speed          = 300.0f;
    float gravityScale   = 0.0f;    // 0 = straight, >0 = arcing down
    float lifetime       = 2.0f;
    int   damage         = 10;
    float hitboxW        = 16.0f;
    float hitboxH        = 16.0f;
    float hitboxOffsetX  = 0.0f;    // hitbox offset from center (flips with facing)
    float hitboxOffsetY  = 0.0f;    // hitbox offset from bottom
    float offsetX        = 100.0f;  // horizontal spawn offset from player (flips with facing)
    float offsetY        = -10.0f;  // vertical spawn offset from player
    float curveAmplitude = 0.0f;    // 0 = straight, >0 = sine wave
    float curveFrequency = 0.0f;
    std::string textureName = "";   // empty = draw circle fallback
    int   frameNum       = 1;
    float frameTime      = 0.1f;
    float scale          = 1.0f;
};
