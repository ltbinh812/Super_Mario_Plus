#pragma once
#include "Entity.h"
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

class EnemyFactory {
public:
    static std::unique_ptr<Entity> create(
        const std::string& identifier,
        Vector2 worldPos,
        const nlohmann::json& fieldInstances);
};
