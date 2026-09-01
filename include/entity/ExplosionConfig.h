#pragma once
#include <string>
#include <functional>

class Entity;

struct ExplosionConfig {
    float lifetime       = 0.5f;
    int   damage         = 20;
    float hitboxW        = 100.0f;
    float hitboxH        = 100.0f;
    float hitboxOffsetX  = 0.0f;
    float hitboxOffsetY  = 0.0f;
    float offsetX        = 50.0f;   // horizontal spawn offset from player (flips with facing)
    float offsetY        = 0.0f;    // vertical spawn offset from player (negative = above)
    std::string textureName = "";
    int   frameNum       = 1;
    float frameTime      = 0.1f;
    float scale          = 1.0f;
    int   hitboxStartFrame = 0;
    int   hitboxEndFrame   = 999;
    std::function<void(Entity*)> onHitEffect = nullptr;
};
