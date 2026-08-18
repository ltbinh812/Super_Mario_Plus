#pragma once
#include "BaseLevelState.h"

class World02State : public BaseLevelState {
public:
    World02State(const std::string& p1Name = "Goku", const std::string& p2Name = "Goku") : BaseLevelState("assets/maps/map02/world02.ldtk", "", p1Name, p2Name) {}
    ~World02State() override = default;
};
