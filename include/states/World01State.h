#pragma once
#include "BaseLevelState.h"

class World01State : public BaseLevelState {
public:
    World01State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map01/world01.ldtk", player1Name) {}
    ~World01State() override = default;
};
