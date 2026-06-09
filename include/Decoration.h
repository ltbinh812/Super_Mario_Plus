#ifndef DECORATION_H
#define DECORATION_H

#include "raylib.h"

class Decoration {
public:
    Decoration(Vector2 pos, int type);
    void Draw() const;

private:
    void DrawBush(float scale) const;
    void DrawTree(float scale) const;
    void DrawPipe(float height) const;

    Vector2 pos_;
    int type_; // 0 = bush, 1 = tree, 2 = pipe
};

#endif
