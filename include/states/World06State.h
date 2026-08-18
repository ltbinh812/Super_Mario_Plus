#pragma once
#include "BaseLevelState.h"

class World06State : public BaseLevelState {
public:
    World06State() : BaseLevelState("assets/maps/map06/world06.ldtk") {}
    ~World06State() override = default;
};
