#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"
#include "Cloud.h"
#include <vector>

class Background {
public:
    static void Draw(float worldWidth, float groundY, const std::vector<Cloud>& clouds);

private:
    static void DrawMountain(Vector2 pos, float w, float h, Color col);
};

#endif
