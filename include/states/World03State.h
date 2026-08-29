#pragma once
#include "BaseLevelState.h"

class World03State : public BaseLevelState {
public:
    World03State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map03/world03.ldtk", "", p1Name) {}
    ~World03State() override = default;
};
