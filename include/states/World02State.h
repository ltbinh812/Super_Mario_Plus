#pragma once
#include "BaseLevelState.h"

class World02State : public BaseLevelState {
public:
    World02State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map02/world02.ldtk", "", p1Name) {}
    ~World02State() override = default;
};
