#pragma once
#include "BaseLevelState.h"

class World04State : public BaseLevelState {
public:
    World04State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map04/world04.ldtk", player1Name) {}
    ~World04State() override = default;
};
