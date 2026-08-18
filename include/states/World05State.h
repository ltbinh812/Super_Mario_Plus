#pragma once
#include "BaseLevelState.h"

class World05State : public BaseLevelState {
public:
    World05State() : BaseLevelState("assets/maps/map05/world05.ldtk") {}
    ~World05State() override = default;
};
