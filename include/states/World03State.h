#pragma once
#include "BaseLevelState.h"

class World03State : public BaseLevelState {
public:
    World03State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map03/world03.ldtk", player1Name) {}
    ~World03State() override = default;
};
