#include "EntityFactory.h"
#include "AssetManager.h"
#include "Fireball.h"
#include "Explosion.h"
#include "FireballConfig.h"
#include "SettingsManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::unique_ptr<Entity> EntityFactory::create(const SpawnCommand& cmd) {
    // Load character data from JSON
    std::ifstream charFile("assets/config/characters.json");
    json jsonData;
    if (charFile.is_open()) {
        charFile >> jsonData;
    } else {
        std::cerr << "[EntityFactory] Cannot open characters.json" << std::endl;
    }

    // Try to also load enemies.json so bosses can spawn projectiles
    std::ifstream enemyFile("assets/config/enemies.json");
    if (enemyFile.is_open()) {
        json enemyData;
        enemyFile >> enemyData;
        // Merge enemyData into jsonData
        for (auto& [key, val] : enemyData.items()) {
            jsonData[key] = val;
        }
    }

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
                cfg.hitboxW        = fb.value("hitboxW", 0.0f);
                cfg.hitboxH        = fb.value("hitboxH", 0.0f);
                cfg.hitboxOffsetX  = fb.value("hitboxOffsetX", 0.0f);
                cfg.hitboxOffsetY  = fb.value("hitboxOffsetY", 0.0f);
                cfg.offsetX        = fb.value("offsetX", 100.0f);
                cfg.offsetY        = fb.value("offsetY", -10.0f);
                cfg.curveAmplitude = fb.value("curveAmplitude", 0.0f);
                cfg.curveFrequency = fb.value("curveFrequency", 0.0f);
                cfg.frameNum       = fb.value("frameNum", 1);
                cfg.frameTime      = fb.value("frameTime", 0.1f);
                cfg.scale          = fb.value("scale", 1.0f);
                cfg.beamFromOwner         = fb.value("beamFromOwner", false);
                cfg.alignFramesByContent  = fb.value("alignFramesByContent", false);

                // Auto-load fireball texture from assetFolder
                std::string texBase = fb.value("textureName", std::string(""));
                if (!texBase.empty()) {
                    if (jsonData[cmd.ownerName].contains("assetFolder")) {
                        std::string folder = jsonData[cmd.ownerName]["assetFolder"].get<std::string>();
                        std::string texKey = cmd.ownerName + "_" + texBase;
                        std::string texPath = "assets/" + folder + "/" + texBase + ".png";
                        AssetManager::getInstance().loadTexture(texKey, texPath);
                        cfg.textureName = texKey;
                        
                        std::string soundPath = "assets/" + folder + "/sounds/" + texBase + ".wav";
                        std::string soundKey = cmd.ownerName + "_" + texBase + "_sound";
                        if (std::filesystem::exists(soundPath)) {
                            AssetManager::getInstance().loadSound(soundKey, soundPath);
                            Sound s = AssetManager::getInstance().getSound(soundKey);
                            float volume = SettingsManager::GetInstance().GetEnemySFXVolume();
                            if (cmd.spawner && cmd.spawner->getFaction() == EntityFaction::Player) {
                                volume = SettingsManager::GetInstance().GetPlayerSFXVolume();
                            }
                            SetSoundVolume(s, volume);
                            PlaySound(s);
                            cfg.soundKey = soundKey;
                        }
                    } else {
                        cfg.textureName = texBase;
                    }
                }
            }

            // Apply spawn offset based on facing direction
            Vector2 spawnPos = cmd.position;
            spawnPos.x += cmd.isFacingRight ? cfg.offsetX : -cfg.offsetX;
            spawnPos.y += cfg.offsetY;

            std::unique_ptr<Entity> entity = std::make_unique<Fireball>(spawnPos, cmd.isFacingRight, cfg, cmd.spawner);
            if (entity) {
                entity->setIid(cmd.iid);
            }
            return entity;
        }

        case EntityType::SpecialBall: {
            FireballConfig cfg;

            // Load special_ball config from owner's json block if available
            if (jsonData.contains(cmd.ownerName) &&
                jsonData[cmd.ownerName].contains("special_ball")) {
                auto& fb = jsonData[cmd.ownerName]["special_ball"];
                cfg.speed          = fb.value("speed", 300.0f);
                cfg.gravityScale   = fb.value("gravityScale", 0.0f);
                cfg.lifetime       = fb.value("lifetime", 2.0f);
                cfg.damage         = fb.value("damage", 10);
                cfg.hitboxW        = fb.value("hitboxW", 0.0f);
                cfg.hitboxH        = fb.value("hitboxH", 0.0f);
                cfg.hitboxOffsetX  = fb.value("hitboxOffsetX", 0.0f);
                cfg.hitboxOffsetY  = fb.value("hitboxOffsetY", 0.0f);
                cfg.offsetX        = fb.value("offsetX", 100.0f);
                cfg.offsetY        = fb.value("offsetY", -10.0f);
                cfg.curveAmplitude = fb.value("curveAmplitude", 0.0f);
                cfg.curveFrequency = fb.value("curveFrequency", 0.0f);
                cfg.frameNum       = fb.value("frameNum", 1);
                cfg.frameTime      = fb.value("frameTime", 0.1f);
                cfg.scale          = fb.value("scale", 1.0f);
                cfg.beamFromOwner         = fb.value("beamFromOwner", false);
                cfg.alignFramesByContent  = fb.value("alignFramesByContent", false);

                // Auto-load texture from assetFolder
                std::string texBase = fb.value("textureName", std::string(""));
                if (!texBase.empty() && jsonData[cmd.ownerName].contains("assetFolder")) {
                    std::string folder = jsonData[cmd.ownerName]["assetFolder"].get<std::string>();
                    std::string texKey = cmd.ownerName + "_" + texBase;
                    std::string texPath = "assets/" + folder + "/" + texBase + ".png";
                    AssetManager::getInstance().loadTexture(texKey, texPath);
                    cfg.textureName = texKey;
                    
                    std::string soundPath = "assets/" + folder + "/sounds/" + texBase + ".wav";
                    std::string soundKey = cmd.ownerName + "_" + texBase + "_sound";
                    if (std::filesystem::exists(soundPath)) {
                        AssetManager::getInstance().loadSound(soundKey, soundPath);
                        Sound s = AssetManager::getInstance().getSound(soundKey);
                        float volume = SettingsManager::GetInstance().GetEnemySFXVolume();
                        if (cmd.spawner && cmd.spawner->getFaction() == EntityFaction::Player) {
                            volume = SettingsManager::GetInstance().GetPlayerSFXVolume();
                        }
                        SetSoundVolume(s, volume);
                        PlaySound(s);
                    }
                }
            }

            Vector2 spawnPos = cmd.position;
            spawnPos.x += cmd.isFacingRight ? cfg.offsetX : -cfg.offsetX;
            spawnPos.y += cfg.offsetY;

            std::unique_ptr<Entity> entity = std::make_unique<Fireball>(spawnPos, cmd.isFacingRight, cfg, cmd.spawner);
            if (entity) {
                entity->setIid(cmd.iid);
            }
            return entity;
        }

        case EntityType::Explosion: {
            ExplosionConfig cfg;

            if (jsonData.contains(cmd.ownerName) &&
                jsonData[cmd.ownerName].contains("explosion")) {
                auto& ex = jsonData[cmd.ownerName]["explosion"];
                cfg.lifetime  = ex.value("lifetime", 0.5f);
                cfg.damage    = ex.value("damage", 20);
                cfg.hitboxW   = ex.value("hitboxW", 0.0f);
                cfg.hitboxH   = ex.value("hitboxH", 0.0f);
                cfg.hitboxOffsetX = ex.value("hitboxOffsetX", 0.0f);
                cfg.hitboxOffsetY = ex.value("hitboxOffsetY", 0.0f);
                cfg.offsetX   = ex.value("offsetX", 50.0f);
                cfg.offsetY   = ex.value("offsetY", 0.0f);
                cfg.frameNum = ex.value("frameNum", 1);
                cfg.frameTime = ex.value("frameTime", 0.1f);
                cfg.scale = ex.value("scale", 1.0f);
                cfg.hitboxStartFrame = ex.value("hitboxStartFrame", 0);
                cfg.hitboxEndFrame = ex.value("hitboxEndFrame", 999);

                // Auto-load texture
                std::string texBase = ex.value("textureName", std::string(""));
                cfg.textureName = texBase;
                if (!texBase.empty() && jsonData[cmd.ownerName].contains("assetFolder")) {
                    std::string folder = jsonData[cmd.ownerName]["assetFolder"].get<std::string>();
                    std::string texKey = cmd.ownerName + "_" + texBase;
                    std::string texPath = "assets/" + folder + "/" + texBase + ".png";
                    AssetManager::getInstance().loadTexture(texKey, texPath);
                    cfg.textureName = texKey;
                    
                    std::string soundPath = "assets/" + folder + "/sounds/" + texBase + ".wav";
                    std::string soundKey = cmd.ownerName + "_" + texBase + "_sound";
                    if (std::filesystem::exists(soundPath)) {
                        AssetManager::getInstance().loadSound(soundKey, soundPath);
                        Sound s = AssetManager::getInstance().getSound(soundKey);
                        float volume = SettingsManager::GetInstance().GetEnemySFXVolume();
                        if (cmd.spawner && cmd.spawner->getFaction() == EntityFaction::Player) {
                            volume = SettingsManager::GetInstance().GetPlayerSFXVolume();
                        }
                        SetSoundVolume(s, volume);
                        PlaySound(s);
                    }
                }
            }
            
            cfg.onHitEffect = cmd.onHitEffect;

            // Apply spawn offset based on facing direction
            Vector2 spawnPos = cmd.position;
            spawnPos.x += cmd.isFacingRight ? cfg.offsetX : -cfg.offsetX;
            spawnPos.y += cfg.offsetY;

            std::unique_ptr<Entity> entity = std::make_unique<Explosion>(spawnPos, cmd.isFacingRight, cfg, cmd.spawner);
            if (entity) {
                entity->setIid(cmd.iid);
            }
            return entity;
        }

        // Future:
        // case EntityType::Shield: { ... }
        // case EntityType::Bomb:   { ... }

        default:
            std::cerr << "[EntityFactory] Unknown EntityType in spawn command" << std::endl;
            return nullptr;
    }
}
