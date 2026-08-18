#pragma once
#include "BaseLevelState.h"

class World01State : public BaseLevelState {
public:
    World01State() : BaseLevelState("assets/maps/map01/world01.ldtk") {}
    ~World01State() override = default;
};
