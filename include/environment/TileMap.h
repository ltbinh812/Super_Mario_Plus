#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <unordered_map>

class TileMap {
private:
    Texture2D tileSheet;
    Texture2D backgroundTex;
    int tileSize;
    int columns;
    int rows;
    int levelWidth;
    int levelHeight;

    std::unordered_map<int, Texture2D> tilesetTextures; // Flyweight: Dùng chung tài nguyên texture theo uid
    RenderTexture2D mapCanvas; // Batch Buffer: Gom toàn bộ background tĩnh vào 1 Canvas
    bool hasCanvas = false;

    std::vector<std::vector<int>> backgroundLayer;
    std::vector<std::vector<int>> displayLayer;
    std::vector<std::vector<int>> collisionLayer;

public:
    TileMap();
    ~TileMap();

    void LoadMap(const std::string& jsonFilePath, const std::string& tilesetPath, const std::string& backgroundPath);
    bool LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName = "");
    
    void Draw() const;
    std::vector<Rectangle> GetCollidingRectangles(Rectangle entityRect) const;

    float GetWorldScale() const { return (tileSize > 0 && tileSize < 32) ? (32.0f / (float)tileSize) : 1.0f; }
    int GetTileSize() const { return tileSize; }
    int GetWidth() const { return (int)(levelWidth * GetWorldScale()); }
    int GetHeight() const { return (int)(levelHeight * GetWorldScale()); }
};

