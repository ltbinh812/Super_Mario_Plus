#ifndef SUPERMARIOPLUS_H
#define SUPERMARIOPLUS_H

#include "raylib.h"

class SuperMarioPlus {
public:
    SuperMarioPlus();
    ~SuperMarioPlus();

    void Init();
    void Update();
    void Draw();
    bool ShouldClose();

private:
    // Game state variables
};

#endif // SUPERMARIOPLUS_H
