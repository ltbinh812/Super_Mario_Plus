#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <unordered_map>

enum class CollisionType {
    None = 0, Solid, OneWay, Hazard, Ladder, Water, Die, Lotus
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
    RenderTexture2D mapCanvas; // Batch Buffer: Gom toàn bộ background tĩnh vào 1 Canvas
    bool hasCanvas = false;

    std::vector<std::vector<int>> backgroundLayer;
    std::vector<std::vector<int>> displayLayer;
    std::vector<std::vector<CollisionType>> collisionLayer;
    std::unordered_map<std::string, std::vector<NeighbourInfo>> currentNeighbours; // Hướng -> Danh sách Level kề



public:
    TileMap();
    ~TileMap();

    void LoadMap(const std::string& jsonFilePath, const std::string& tilesetPath, const std::string& backgroundPath);
    bool LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName = "");
    
    void Draw() const;
    std::vector<CollisionTile> GetCollidingTiles(Rectangle entityRect) const;
    std::string GetNeighbour(const std::string& dir, float globalX, float globalY) const;



    float GetWorldScale() const { return (tileSize > 0 && tileSize < 32) ? (32.0f / (float)tileSize) : 1.0f; }
    int GetTileSize() const { return tileSize; }
    int GetWidth() const { return (int)(levelWidth * GetWorldScale()); }
    int GetHeight() const { return (int)(levelHeight * GetWorldScale()); }
    int GetWorldX() const { return (int)(worldX * GetWorldScale()); }
    int GetWorldY() const { return (int)(worldY * GetWorldScale()); }
};

