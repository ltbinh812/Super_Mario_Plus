#include "EntityFactory.h"
#include "Fireball.h"
#include "FireballConfig.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::unique_ptr<Entity> EntityFactory::create(const SpawnCommand& cmd) {
    // Load character data from JSON
    std::ifstream file("assets/config/characters.json");
    if (!file.is_open()) {
        std::cerr << "[EntityFactory] Cannot open characters.json" << std::endl;
        return nullptr;
    }

    json jsonData;
    file >> jsonData;

    switch (cmd.type) {
        case EntityType::Fireball: {
            FireballConfig cfg;  // defaults apply if no JSON block

            if (jsonData.contains(cmd.ownerName) &&
                jsonData[cmd.ownerName].contains("fireball")) {
                auto& fb = jsonData[cmd.ownerName]["fireball"];
                cfg.speed          = fb.value("speed", 300.0f);
                cfg.gravityScale   = fb.value("gravityScale", 0.0f);
                cfg.lifetime       = fb.value("lifetime", 2.0f);
                cfg.damage         = fb.value("damage", 10);
                cfg.hitboxW        = fb.value("hitboxW", 16.0f);
                cfg.hitboxH        = fb.value("hitboxH", 16.0f);
                cfg.curveAmplitude = fb.value("curveAmplitude", 0.0f);
                cfg.curveFrequency = fb.value("curveFrequency", 0.0f);
                cfg.textureName    = fb.value("textureName", std::string(""));
                cfg.frameNum       = fb.value("frameNum", 1);
                cfg.frameTime      = fb.value("frameTime", 0.1f);
            }

            return std::make_unique<Fireball>(cmd.position, cmd.isFacingRight, cfg, cmd.spawner);
        }

        // Future:
        // case EntityType::Shield: { ... }
        // case EntityType::Bomb:   { ... }

        default:
            std::cerr << "[EntityFactory] Unknown EntityType in spawn command" << std::endl;
            return nullptr;
    }
}
