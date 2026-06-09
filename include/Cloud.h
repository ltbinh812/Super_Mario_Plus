#ifndef CLOUD_H
#define CLOUD_H

#include "raylib.h"

class Cloud {
public:
    Cloud(Vector2 pos, float speed, float scale);
    void Update(float dt, float worldWidth);
    void Draw() const;

private:
    Vector2 pos_;
    float speed_;
    float scale_;
};

#endif
