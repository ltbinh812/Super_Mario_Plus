#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

// Flyweight Singleton: Loads all item atlases once and shares them across all item instances.
// Atlases: "a" (a.json/a.png), "buff" (buff.json/buff.png),
//          "chest_normal" (chest_normal.json/chest_normal.png),
//          "chest_boss" (chest_boss.json/chest_boss.png)
class ItemAtlasRegistry {
public:
    static ItemAtlasRegistry& getInstance();

    // Call once at startup with the path to the item folder (e.g. "assets/maps/item/")
    void loadAll(const std::string& itemDir);
    void unloadAll();

    // Returns the source rectangle for a given frame name (e.g. "coin_gold.png")
    Rectangle getFrame(const std::string& frameName) const;

    // Returns the texture for the atlas that contains the given frame name
    const Texture2D& getTexture(const std::string& frameName) const;

    bool isLoaded() const { return loaded_; }

    // Loads a single atlas: reads the .json for frame rects, loads the .png texture
    void loadAtlas(const std::string& name, const std::string& jsonPath, const std::string& pngPath);

private:
    ItemAtlasRegistry() = default;
    ~ItemAtlasRegistry() = default;
    ItemAtlasRegistry(const ItemAtlasRegistry&) = delete;
    ItemAtlasRegistry& operator=(const ItemAtlasRegistry&) = delete;

    std::unordered_map<std::string, Texture2D> textures_;       // atlas name -> Texture
    std::unordered_map<std::string, Rectangle> frames_;         // frame name -> src rect
    std::unordered_map<std::string, std::string> frameToAtlas_; // frame name -> atlas name

    bool loaded_ = false;
};
