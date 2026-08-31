#pragma once
#include "BaseItem.h"
#include <memory>
#include <string>
#include "nlohmann/json.hpp"

class ItemFactory {
public:
    static std::unique_ptr<BaseItem> create(
        const std::string& identifier,
        Vector2 worldPos,
        const nlohmann::json& fieldInstances
    );

    // Overload for spawning items dynamically during gameplay without LDtk data
    static std::unique_ptr<BaseItem> createDynamic(
        const std::string& identifier,
        Vector2 worldPos
    );

    // Overload for spawning items with initial velocity (e.g. ThrownBoom)
    static std::unique_ptr<BaseItem> createDynamic(
        const std::string& identifier,
        Vector2 worldPos,
        Vector2 initialVelocity
    );
};
