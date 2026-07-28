#include "TileMap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

TileMap::TileMap() : tileSize(16), columns(0), rows(0), levelWidth(0), levelHeight(0) {
    tileSheet.id = 0;
    backgroundTex.id = 0;
}

TileMap::~TileMap() {
    if (tileSheet.id != 0) UnloadTexture(tileSheet);
    if (backgroundTex.id != 0) UnloadTexture(backgroundTex);
    for (auto& pair : tilesetTextures) {
        if (pair.second.id != 0) UnloadTexture(pair.second);
    }
    if (hasCanvas) UnloadRenderTexture(mapCanvas);
}

void TileMap::LoadMap(const std::string& jsonFilePath, const std::string& tilesetPath, const std::string& backgroundPath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << jsonFilePath << std::endl;
        return;
    }

    json j;
    file >> j;

    tileSize = j["mapSize"]["tileSize"];
    columns = j["mapSize"]["columns"];
    rows = j["mapSize"]["rows"];
    
    int tilesetColumns = j["tilesets"]["columns"];

    // Initialize arrays with -1 (empty)
    backgroundLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, -1));
    displayLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, -1));
    collisionLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, 0));

    // Parse objects
    for (const auto& obj : j["objects"]) {
        int baseId = obj["baseId"];
        int startX = obj["x"];
        int startY = obj["y"];
        int objWidth = obj["width"];
        int objHeight = obj["height"];
        
        int srcWidth = objWidth;
        int srcHeight = objHeight;
        if (obj.contains("sourceWidth")) srcWidth = obj["sourceWidth"];
        if (obj.contains("sourceHeight")) srcHeight = obj["sourceHeight"];
        
        bool solid = obj.contains("solid") ? (bool)obj["solid"] : false;
        bool nineSlice = obj.contains("nineSlice") ? (bool)obj["nineSlice"] : false;

        for (int i = 0; i < objHeight; ++i) {
            for (int j = 0; j < objWidth; ++j) {
                int py = startY + i;
                int px = startX + j;
                if (py >= 0 && py < rows && px >= 0 && px < columns) {
                    int localY = i % srcHeight;
                    int localX = j % srcWidth;

                    if (nineSlice) {
                        if (srcWidth == 3) {
                            if (j == 0) localX = 0;
                            else if (j == objWidth - 1) localX = 2;
                            else localX = 1;
                        }
                        if (srcHeight == 3) {
                            if (i == 0) localY = 0;
                            else if (i == objHeight - 1) localY = 2;
                            else localY = 1;
                        }
                    }

                    int tileId = baseId + (localY * tilesetColumns) + localX;
                    
                    displayLayer[py][px] = tileId;
                    if (solid) {
                        collisionLayer[py][px] = 1;
                    }
                }
            }
        }
    }

    tileSheet = LoadTexture(tilesetPath.c_str());
    backgroundTex = LoadTexture(backgroundPath.c_str());
}

bool TileMap::LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName) {
    std::ifstream file(ldtkFilePath);
    if (!file.is_open()) {
        std::cerr << "[LDtk] Khong the mo file: " << ldtkFilePath << std::endl;
        return false;
    }

    json j;
    file >> j;

    std::string baseDir = ldtkFilePath.substr(0, ldtkFilePath.find_last_of("/\\") + 1);
    for (const auto& ts : j["defs"]["tilesets"]) {
        if (!ts.contains("relPath") || ts["relPath"].is_null()) continue;
        int uid = ts["uid"];
        std::string relPath = ts["relPath"];
        
        std::string filename = relPath;
        size_t lastSlash = relPath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            filename = relPath.substr(lastSlash + 1);
        }
        std::string localPath = baseDir + filename;
        std::string fullPath = baseDir + relPath;

        if (FileExists(localPath.c_str())) {
            tilesetTextures[uid] = LoadTexture(localPath.c_str());
            std::cout << "[LDtk] Da tai Texture ID " << uid << " (Smart Local): " << filename << "\n";
        } else {
            tilesetTextures[uid] = LoadTexture(fullPath.c_str());
            std::cout << "[LDtk] Da tai Texture ID " << uid << ": " << relPath << "\n";
        }
    }

    json targetLevel = nullptr;
    for (const auto& lvl : j["levels"]) {
        if ((lvl.contains("identifier") && !lvl["identifier"].is_null() && lvl["identifier"] == levelName) || levelName.empty()) {
            targetLevel = lvl;
            break;
        }
    }

    if (targetLevel.is_null() && !j["levels"].empty()) {
        targetLevel = j["levels"][0];
    }

    if (targetLevel.is_null()) {
        std::cerr << "[LDtk] Khong tim thay level: " << levelName << std::endl;
        return false;
    }
    std::string actualLevelName = targetLevel.contains("identifier") && !targetLevel["identifier"].is_null() ? (std::string)targetLevel["identifier"] : "Unknown";
    std::cout << "[LDtk] Da chon Level: " << actualLevelName << "\n";

    levelWidth = targetLevel["pxWid"];
    levelHeight = targetLevel["pxHei"];
    
    auto& layerInstances = targetLevel["layerInstances"];
    if (!layerInstances.empty()) {
        tileSize = layerInstances[0]["__gridSize"];
        columns = layerInstances[0]["__cWid"];
        rows = layerInstances[0]["__cHei"];
    }

    collisionLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, 0));

    if (hasCanvas) UnloadRenderTexture(mapCanvas);
    mapCanvas = LoadRenderTexture(levelWidth, levelHeight);
    hasCanvas = true;

    BeginTextureMode(mapCanvas);
    ClearBackground(BLACK); // Nền đen mặc định cho phần không có tài nguyên/không vẽ của map

    for (int i = (int)layerInstances.size() - 1; i >= 0; --i) {
        const auto& layer = layerInstances[i];
        std::string layerName = (layer.contains("__identifier") && !layer["__identifier"].is_null()) ? (std::string)layer["__identifier"] : "";
        std::string layerType = (layer.contains("__type") && !layer["__type"].is_null()) ? (std::string)layer["__type"] : "";

        if (layerName == "Collision") {
            if (layer.contains("intGridCsv") && !layer["intGridCsv"].is_null()) {
                const auto& csv = layer["intGridCsv"];
                for (size_t idx = 0; idx < csv.size(); ++idx) {
                    int val = csv[idx];
                    if (val > 0) {
                        int y = idx / columns;
                        int x = idx % columns;
                        if (y < rows && x < columns) {
                            collisionLayer[y][x] = val;
                        }
                    }
                }
            }
            continue; // Không render lớp Collision ra màn hình
        }

        if (layerType == "Entities") {
            if (layer.contains("entityInstances") && !layer["entityInstances"].is_null()) {
                for (const auto& ent : layer["entityInstances"]) {
                    if (!ent.contains("__tile") || ent["__tile"].is_null()) continue;
                    
                    const auto& tileInfo = ent["__tile"];
                    int tsUid = tileInfo["tilesetUid"];
                    if (tilesetTextures.find(tsUid) == tilesetTextures.end()) continue;

                    Texture2D tex = tilesetTextures[tsUid];
                    Rectangle src = {
                        (float)tileInfo["x"], (float)tileInfo["y"],
                        (float)tileInfo["w"], (float)tileInfo["h"]
                    };
                    Vector2 pos = { (float)ent["px"][0], (float)ent["px"][1] };

                    float pivotX = ent.contains("__pivot") && !ent["__pivot"].is_null() ? (float)ent["__pivot"][0] : 0.0f;
                    float pivotY = ent.contains("__pivot") && !ent["__pivot"].is_null() ? (float)ent["__pivot"][1] : 0.0f;
                    pos.x -= src.width * pivotX;
                    pos.y -= src.height * pivotY;

                    DrawTextureRec(tex, src, pos, WHITE);
                }
            }
        }

        const auto& tiles = (layer.contains("autoLayerTiles") && !layer["autoLayerTiles"].is_null() && !layer["autoLayerTiles"].empty()) 
                            ? layer["autoLayerTiles"] 
                            : ((layer.contains("gridTiles") && !layer["gridTiles"].is_null()) ? layer["gridTiles"] : json::array());

        int tsDefUid = layer["__tilesetDefUid"].is_null() ? -1 : (int)layer["__tilesetDefUid"];
        if (tsDefUid != -1 && tilesetTextures.find(tsDefUid) != tilesetTextures.end()) {
            Texture2D tex = tilesetTextures[tsDefUid];
            for (const auto& tile : tiles) {
                int px = tile["px"][0];
                int py = tile["px"][1];
                int srcX = tile["src"][0];
                int srcY = tile["src"][1];
                int f = tile["f"];

                Rectangle srcRect = { (float)srcX, (float)srcY, (float)tileSize, (float)tileSize };
                if (f == 1 || f == 3) srcRect.width = -srcRect.width;
                if (f == 2 || f == 3) srcRect.height = -srcRect.height;

                DrawTextureRec(tex, srcRect, { (float)px, (float)py }, WHITE);
            }
        }
    }
    EndTextureMode();

    std::cout << "[LDtk] Load map " << actualLevelName << " (" << levelWidth << "x" << levelHeight << ") thanh cong!\n";
    return true;
}

void TileMap::Draw() const {
    if (hasCanvas) {
        float scale = GetWorldScale();
        Rectangle src = { 0.0f, 0.0f, (float)mapCanvas.texture.width, -(float)mapCanvas.texture.height };
        Rectangle dest = { 0.0f, 0.0f, (float)mapCanvas.texture.width * scale, (float)mapCanvas.texture.height * scale };
        DrawTexturePro(mapCanvas.texture, src, dest, { 0.0f, 0.0f }, 0.0f, WHITE);
        return;
    }

    // 1. Draw parallax background (scale to fit the screen roughly or repeat)
    if (backgroundTex.id != 0) {
        // Just drawing it at 0,0 for now, scale by 2.0 to make it cover more area
        DrawTextureEx(backgroundTex, {0, 0}, 0.0f, 2.0f, WHITE);
    }

    if (tileSheet.id == 0) return;

    int tilesX = tileSheet.width / tileSize;
    float scale = GetWorldScale();

    // Helper lambda to draw a layer
    auto drawLayer = [&](const std::vector<std::vector<int>>& layer) {
        for (int y = 0; y < layer.size(); ++y) {
            for (int x = 0; x < layer[y].size(); ++x) {
                int tileId = layer[y][x];
                if (tileId < 0) continue; // Empty tile

                int srcX = (tileId % tilesX) * tileSize;
                int srcY = (tileId / tilesX) * tileSize;

                Rectangle sourceRect = { (float)srcX, (float)srcY, (float)tileSize, (float)tileSize };
                Rectangle destRect = { (float)(x * tileSize) * scale, (float)(y * tileSize) * scale, (float)tileSize * scale, (float)tileSize * scale };

                DrawTexturePro(tileSheet, sourceRect, destRect, { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }
    };

    // 2. Draw background tiles (trees, etc.)
    drawLayer(backgroundLayer);

    // 3. Draw display tiles (ground, floating blocks)
    drawLayer(displayLayer);
}

std::vector<Rectangle> TileMap::GetCollidingRectangles(Rectangle entityRect) const {
    std::vector<Rectangle> collisions;
    if (tileSize == 0 || columns == 0 || rows == 0) return collisions;
    
    float scale = GetWorldScale();
    float worldTileSize = (float)tileSize * scale;

    int startX = std::max(0, (int)(entityRect.x / worldTileSize));
    int startY = std::max(0, (int)(entityRect.y / worldTileSize));
    int endX = std::min(columns - 1, (int)((entityRect.x + entityRect.width) / worldTileSize));
    int endY = std::min(rows - 1, (int)((entityRect.y + entityRect.height) / worldTileSize));

    for (int y = startY; y <= endY; ++y) {
        if (y >= collisionLayer.size()) continue;
        for (int x = startX; x <= endX; ++x) {
            if (x >= collisionLayer[y].size()) continue;
            
            int tileId = collisionLayer[y][x];
            // If the collision layer has a value > 0, it's a solid block
            if (tileId > 0) {
                Rectangle tileRect = { (float)(x * worldTileSize), (float)(y * worldTileSize), worldTileSize, worldTileSize };
                if (CheckCollisionRecs(entityRect, tileRect)) {
                    collisions.push_back(tileRect);
                }
            }
        }
    }
    return collisions;
}
