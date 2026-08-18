#pragma once
#include "BaseLevelState.h"

class World06State : public BaseLevelState {
public:
    World06State(const std::string& p1Name = "Goku", const std::string& p2Name = "Goku") : BaseLevelState("assets/maps/map06/world06.ldtk", "", p1Name, p2Name) {}
    ~World06State() override = default;
};
