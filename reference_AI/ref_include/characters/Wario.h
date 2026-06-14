#ifndef WARIO_H
#define WARIO_H

#include "Player.h"

class Wario : public Player {
public:
    Wario();
    void Draw() override;
};

#endif
