#pragma once
#include "BaseLevelState.h"

class World05State : public BaseLevelState {
public:
    World05State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map05/world05.ldtk", "", p1Name) {}
    ~World05State() override = default;
};
