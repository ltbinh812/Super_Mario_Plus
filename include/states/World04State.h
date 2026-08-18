#pragma once
#include "BaseLevelState.h"

class World04State : public BaseLevelState {
public:
    World04State() : BaseLevelState("assets/maps/map04/world04.ldtk") {}
    ~World04State() override = default;
};
