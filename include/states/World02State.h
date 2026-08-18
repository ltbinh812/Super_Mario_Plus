#pragma once
#include "BaseLevelState.h"

class World02State : public BaseLevelState {
public:
    World02State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map02/world02.ldtk", player1Name) {}
    ~World02State() override = default;
};
