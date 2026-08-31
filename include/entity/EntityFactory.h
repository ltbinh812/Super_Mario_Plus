#pragma once
#include "SpawnCommand.h"
#include "Entity.h"
#include <memory>

class EntityFactory {
public:
    // Creates an entity from a spawn command, reading config from characters.json
    static std::unique_ptr<Entity> create(const SpawnCommand& cmd);
};
