#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"

class Platform {
public:
    Platform(Rectangle r, Color c);
    void Draw() const;
    Rectangle GetBounds() const { return rect_; }

private:
    Rectangle rect_;
    Color color_;
};

#endif
