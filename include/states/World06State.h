#pragma once
#include "BaseLevelState.h"

class World06State : public BaseLevelState {
public:
    World06State(const std::string& player1Name = "goku") : BaseLevelState("assets/maps/map06/world06.ldtk", player1Name) {}
    ~World06State() override = default;
};
