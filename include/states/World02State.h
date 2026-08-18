#pragma once
#include "BaseLevelState.h"

class World02State : public BaseLevelState {
public:
    World02State() : BaseLevelState("assets/maps/map02/world02.ldtk") {}
    ~World02State() override = default;
};
