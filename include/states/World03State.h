#pragma once
#include "BaseLevelState.h"

class World03State : public BaseLevelState {
public:
    World03State() : BaseLevelState("assets/maps/map03/world03.ldtk") {}
    ~World03State() override = default;
};
