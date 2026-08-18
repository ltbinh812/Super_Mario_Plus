#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "nlohmann/json.hpp"

enum class CollisionType {
    None = 0, Solid, OneWay, Hazard, Ladder, Water, Die, Lotus, Cloud, Poison, Lava, Slop
};

struct CollisionTile {
    Rectangle rect;
    CollisionType type;
};

struct NeighbourInfo {
    std::string levelName;
    int worldX;
    int worldY;
    int width;
    int height;
};

// Data from a single LDtk Entity instance
struct LDtkEntityData {
    std::string identifier;          // e.g. "Spring_down", "Coin"
    Vector2     px;                  // position in LDTK pixels (un-scaled)
    std::string iid;                 // unique instance ID for state persistence
    nlohmann::json fieldInstances;   // raw fieldInstances array
};

class TileMap {

private:
    Texture2D tileSheet;
    Texture2D backgroundTex;
    int tileSize;
    int columns;
    int rows;
    int levelWidth;
    int levelHeight;
    int worldX = 0;
    int worldY = 0;

    std::unordered_map<int, Texture2D> tilesetTextures; // Flyweight: Dùng chung tài nguyên texture theo uid
    Texture2D backgroundTexture;
    bool hasBackgroundTexture = false;
    RenderTexture2D mapCanvas; // Batch Buffer: Gom toàn bộ background tĩnh vào 1 Canvas
    bool hasCanvas = false;

    std::vector<std::vector<int>> backgroundLayer;
    std::vector<std::vector<int>> displayLayer;
    std::vector<std::vector<CollisionType>> collisionLayer;
    std::unordered_map<std::string, std::vector<NeighbourInfo>> currentNeighbours; // Hướng -> Danh sách Level kề
    std::vector<Vector2> playerSpawns;
    std::vector<LDtkEntityData> entityData_;
    std::string currentLevelName;



public:
    TileMap();
    ~TileMap();

    void LoadMap(const std::string& jsonFilePath, const std::string& tilesetPath, const std::string& backgroundPath);
    bool LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName = "");
    
    void Draw() const;
    std::vector<CollisionTile> GetCollidingTiles(Rectangle entityRect) const;
    std::string GetNeighbour(const std::string& dir, float globalX, float globalY) const;
    std::vector<Vector2> GetPlayerSpawns() const;
    std::vector<LDtkEntityData> GetEntityData() const;
    std::string GetCurrentLevelName() const { return currentLevelName; }


    float GetWorldScale() const { return (tileSize > 0 && tileSize < 32) ? (32.0f / (float)tileSize) : 1.0f; }
    int GetTileSize() const { return tileSize; }
    int GetWidth() const { return (int)(levelWidth * GetWorldScale()); }
    int GetHeight() const { return (int)(levelHeight * GetWorldScale()); }
    int GetWorldX() const { return (int)(worldX * GetWorldScale()); }
    int GetWorldY() const { return (int)(worldY * GetWorldScale()); }
};

