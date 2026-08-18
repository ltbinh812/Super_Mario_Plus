#pragma once
#include "BaseLevelState.h"

class World01State : public BaseLevelState {
public:
    World01State(const std::string& p1Name = "Goku", const std::string& p2Name = "Goku") : BaseLevelState("assets/maps/map01/world01.ldtk", "", p1Name, p2Name) {}
    ~World01State() override = default;
};
