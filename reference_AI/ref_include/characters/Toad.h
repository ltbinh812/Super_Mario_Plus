#ifndef TOAD_H
#define TOAD_H

#include "Player.h"

class Toad : public Player {
public:
    Toad();
    void Draw() override;
};

#endif
