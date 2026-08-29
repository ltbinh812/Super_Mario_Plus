#pragma once
#include "BaseLevelState.h"

class World06State : public BaseLevelState {
public:
    World06State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map06/world06.ldtk", "", p1Name) {}
    ~World06State() override = default;
};
