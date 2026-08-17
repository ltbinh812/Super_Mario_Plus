#include "EntityFactory.h"
#include "AssetManager.h"
#include "Fireball.h"
#include "Explosion.h"
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
                cfg.offsetX        = fb.value("offsetX", 100.0f);
                cfg.offsetY        = fb.value("offsetY", -10.0f);
                cfg.curveAmplitude = fb.value("curveAmplitude", 0.0f);
                cfg.curveFrequency = fb.value("curveFrequency", 0.0f);
                cfg.frameNum       = fb.value("frameNum", 1);
                cfg.frameTime      = fb.value("frameTime", 0.1f);

                // Auto-load fireball texture from assetFolder
                std::string texBase = fb.value("textureName", std::string(""));
                if (!texBase.empty() && jsonData[cmd.ownerName].contains("assetFolder")) {
                    std::string folder = jsonData[cmd.ownerName]["assetFolder"].get<std::string>();
                    std::string texKey = cmd.ownerName + "_" + texBase;
                    std::string texPath = "assets/" + folder + "/" + texBase + ".png";
                    AssetManager::getInstance().loadTexture(texKey, texPath);
                    cfg.textureName = texKey;
                }
            }

            // Apply spawn offset based on facing direction
            Vector2 spawnPos = cmd.position;
            spawnPos.x += cmd.isFacingRight ? cfg.offsetX : -cfg.offsetX;
            spawnPos.y += cfg.offsetY;

            return std::make_unique<Fireball>(spawnPos, cmd.isFacingRight, cfg, cmd.spawner);
        }

        case EntityType::Explosion: {
            ExplosionConfig cfg;

            if (jsonData.contains(cmd.ownerName) &&
                jsonData[cmd.ownerName].contains("explosion")) {
                auto& ex = jsonData[cmd.ownerName]["explosion"];
                cfg.lifetime  = ex.value("lifetime", 0.5f);
                cfg.damage    = ex.value("damage", 20);
                cfg.hitboxW   = ex.value("hitboxW", 100.0f);
                cfg.hitboxH   = ex.value("hitboxH", 100.0f);
                cfg.offsetX   = ex.value("offsetX", 50.0f);
                cfg.offsetY   = ex.value("offsetY", 0.0f);
                cfg.frameNum  = ex.value("frameNum", 1);
                cfg.frameTime = ex.value("frameTime", 0.1f);

                // Auto-load texture
                std::string texBase = ex.value("textureName", std::string(""));
                if (!texBase.empty() && jsonData[cmd.ownerName].contains("assetFolder")) {
                    std::string folder = jsonData[cmd.ownerName]["assetFolder"].get<std::string>();
                    std::string texKey = cmd.ownerName + "_" + texBase;
                    std::string texPath = "assets/" + folder + "/" + texBase + ".png";
                    AssetManager::getInstance().loadTexture(texKey, texPath);
                    cfg.textureName = texKey;
                }
            }

            // Apply spawn offset based on facing direction
            Vector2 spawnPos = cmd.position;
            spawnPos.x += cmd.isFacingRight ? cfg.offsetX : -cfg.offsetX;
            spawnPos.y += cfg.offsetY;

            return std::make_unique<Explosion>(spawnPos, cfg, cmd.spawner);
        }

        // Future:
        // case EntityType::Shield: { ... }
        // case EntityType::Bomb:   { ... }

        default:
            std::cerr << "[EntityFactory] Unknown EntityType in spawn command" << std::endl;
            return nullptr;
    }
}
