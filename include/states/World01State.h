#pragma once
#include "BaseLevelState.h"

class World01State : public BaseLevelState {
public:
    World01State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map01/world01.ldtk", "", p1Name) {}
    ~World01State() override = default;
};
