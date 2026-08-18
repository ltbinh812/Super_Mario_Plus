#include "ItemAtlasRegistry.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

ItemAtlasRegistry& ItemAtlasRegistry::getInstance() {
    static ItemAtlasRegistry instance;
    return instance;
}

void ItemAtlasRegistry::loadAll(const std::string& itemDir) {
    if (loaded_) return;

    // Load each known atlas (name, json relative path, png relative path)
    loadAtlas("a",            itemDir + "a.json",            itemDir + "a.png");
    loadAtlas("buff",         itemDir + "buff.json",         itemDir + "buff.png");
    loadAtlas("chest_normal", itemDir + "chest_normal.json", itemDir + "chest_normal.png");
    loadAtlas("chest_boss",   itemDir + "chest_boss.json",   itemDir + "chest_boss.png");
    loadAtlas("explosion",    itemDir + "explosion.json",    itemDir + "explosion.png");
    loadAtlas("fire_effect",  itemDir + "fire_effect.json",  itemDir + "fire_effect.png");
    loadAtlas("poison_effect1",   itemDir + "poison_effect1.json",   itemDir + "poison_effect1.png");
    loadAtlas("poison_effect2",   itemDir + "poison_effect2.json",   itemDir + "poison_effect2.png");

    loaded_ = true;
    std::cout << "[ItemAtlasRegistry] Loaded " << frames_.size() << " frames from " << textures_.size() << " atlases.\n";
}

void ItemAtlasRegistry::loadAtlas(const std::string& name, const std::string& jsonPath, const std::string& pngPath) {
    // Load texture
    Texture2D tex = LoadTexture(pngPath.c_str());
    if (tex.id == 0) {
        std::cerr << "[ItemAtlasRegistry] Failed to load texture: " << pngPath << "\n";
        return;
    }
    textures_[name] = tex;

    // Parse JSON for frame rects
    std::ifstream f(jsonPath);
    if (!f.is_open()) {
        std::cerr << "[ItemAtlasRegistry] Failed to open JSON: " << jsonPath << "\n";
        return;
    }

    json j;
    try {
        f >> j;
    } catch (const std::exception& e) {
        std::cerr << "[ItemAtlasRegistry] JSON parse error (" << jsonPath << "): " << e.what() << "\n";
        return;
    }

    if (!j.contains("frames")) return;

    for (auto& [frameName, frameData] : j["frames"].items()) {
        if (!frameData.contains("frame")) continue;
        Rectangle rect = {
            (float)frameData["frame"]["x"],
            (float)frameData["frame"]["y"],
            (float)frameData["frame"]["w"],
            (float)frameData["frame"]["h"]
        };
        frames_[frameName] = rect;
        frameToAtlas_[frameName] = name;
    }
}

Rectangle ItemAtlasRegistry::getFrame(const std::string& frameName) const {
    auto it = frames_.find(frameName);
    if (it == frames_.end()) {
        std::cerr << "[ItemAtlasRegistry] Frame not found: " << frameName << "\n";
        return { 0, 0, 16, 16 };
    }
    return it->second;
}

const Texture2D& ItemAtlasRegistry::getTexture(const std::string& frameName) const {
    auto atlasIt = frameToAtlas_.find(frameName);
    if (atlasIt == frameToAtlas_.end()) {
        std::cerr << "[ItemAtlasRegistry] No atlas for frame: " << frameName << "\n";
        static Texture2D dummy{};
        return dummy;
    }
    auto texIt = textures_.find(atlasIt->second);
    if (texIt == textures_.end()) {
        static Texture2D dummy{};
        return dummy;
    }
    return texIt->second;
}

void ItemAtlasRegistry::unloadAll() {
    for (auto& [name, tex] : textures_) {
        if (tex.id != 0) UnloadTexture(tex);
    }
    textures_.clear();
    frames_.clear();
    frameToAtlas_.clear();
    loaded_ = false;
}
