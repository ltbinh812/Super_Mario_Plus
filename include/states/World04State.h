#pragma once
#include "BaseLevelState.h"

class World04State : public BaseLevelState {
public:
    World04State(const std::string& p1Name = "Goku", const std::string& p2Name = "Goku") : BaseLevelState("assets/maps/map04/world04.ldtk", "", p1Name, p2Name) {}
    ~World04State() override = default;
};
