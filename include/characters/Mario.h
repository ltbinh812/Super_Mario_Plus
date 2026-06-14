#ifndef MARIO_H
#define MARIO_H

#include "Player.h"

class Mario : public Player {
public:
    Mario();
    void Draw() override;
};

#endif
