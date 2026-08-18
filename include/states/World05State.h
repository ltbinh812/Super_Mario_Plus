#pragma once
#include "BaseLevelState.h"

class World05State : public BaseLevelState {
public:
    World05State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map05/world05.ldtk", player1Name) {}
    ~World05State() override = default;
};
