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
    if (hasBackgroundTexture) UnloadTexture(backgroundTexture);
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

    // Initialize arrays with -1 (empty) for visuals, None for collision
    backgroundLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, -1));
    displayLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, -1));
    collisionLayer = std::vector<std::vector<CollisionType>>(rows, std::vector<CollisionType>(columns, CollisionType::None));

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
                        collisionLayer[py][px] = CollisionType::Solid;
                    }

                }
            }
        }
    }

    tileSheet = LoadTexture(tilesetPath.c_str());
    backgroundTex = LoadTexture(backgroundPath.c_str());
}

bool TileMap::LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName) {
    playerSpawns.clear();
    entityData_.clear();
    std::ifstream file(ldtkFilePath);
    if (!file.is_open()) {
        std::cerr << "[LDtk] Khong the mo file: " << ldtkFilePath << std::endl;
        return false;
    }

    json j;
    file >> j;

    std::string baseDir = ldtkFilePath.substr(0, ldtkFilePath.find_last_of("/\\") + 1);
    
    // Tạo map ánh xạ IntGrid value sang CollisionType
    std::unordered_map<int, CollisionType> intToCollisionType;
    if (j.contains("defs") && j["defs"].contains("layers")) {
        for (const auto& layerDef : j["defs"]["layers"]) {
            if (layerDef.contains("identifier") && (layerDef["identifier"] == "Collision" || layerDef["identifier"] == "Collisions")) {
                if (layerDef.contains("intGridValues")) {
                    for (const auto& valDef : layerDef["intGridValues"]) {
                        int value = valDef["value"];
                        std::string id = valDef["identifier"];
                        CollisionType type = CollisionType::None;
                        if (id == "Solid") type = CollisionType::Solid;
                        else if (id == "OneWay") type = CollisionType::OneWay;
                        else if (id == "Hazard") type = CollisionType::Hazard;
                        else if (id == "Ladder") type = CollisionType::Ladder;
                        else if (id == "Water") type = CollisionType::Water;
                        else if (id == "Die") type = CollisionType::Die;
                        else if (id == "hoa_sen" || id == "Lotus") type = CollisionType::Lotus;
                        else if (id == "Cloud") type = CollisionType::Cloud;
                        else if (id == "Poison") type = CollisionType::Poison;
                        else if (id == "Lava") type = CollisionType::Lava;
                        else if (id == "Slop") type = CollisionType::Slop;
                        intToCollisionType[value] = type;
                    }
                }
                break;
            }
        }
    }

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
    if (levelName.empty()) {
        for (const auto& lvl : j["levels"]) {
            bool foundStart = false;
            if (lvl.contains("layerInstances") && !lvl["layerInstances"].is_null()) {
                for (const auto& layer : lvl["layerInstances"]) {
                    if (layer.contains("__type") && layer["__type"] == "Entities" && layer.contains("entityInstances") && !layer["entityInstances"].is_null()) {
                        for (const auto& ent : layer["entityInstances"]) {
                            if (ent.contains("__identifier") && ent["__identifier"] == "Starting_position") {
                                foundStart = true;
                                break;
                            }
                        }
                    }
                    if (foundStart) break;
                }
            }
            if (foundStart) {
                targetLevel = lvl;
                break;
            }
        }
    } else {
        for (const auto& lvl : j["levels"]) {
            if (lvl.contains("identifier") && !lvl["identifier"].is_null() && lvl["identifier"] == levelName) {
                targetLevel = lvl;
                break;
            }
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
    currentLevelName = actualLevelName;
    std::cout << "[LDtk] Da chon Level: " << actualLevelName << "\n";

    if (hasBackgroundTexture) {
        UnloadTexture(backgroundTexture);
        hasBackgroundTexture = false;
    }
    if (targetLevel.contains("bgRelPath") && !targetLevel["bgRelPath"].is_null()) {
        std::string bgRel = targetLevel["bgRelPath"];
        std::string filename = bgRel;
        size_t lastSlash = bgRel.find_last_of("/\\");
        if (lastSlash != std::string::npos) filename = bgRel.substr(lastSlash + 1);
        std::string localPath = baseDir + filename;
        std::string fullPath = baseDir + bgRel;

        if (FileExists(localPath.c_str())) {
            backgroundTexture = LoadTexture(localPath.c_str());
            hasBackgroundTexture = true;
        } else if (FileExists(fullPath.c_str())) {
            backgroundTexture = LoadTexture(fullPath.c_str());
            hasBackgroundTexture = true;
        }
    }

    currentNeighbours.clear();
    if (targetLevel.contains("__neighbours") && !targetLevel["__neighbours"].is_null()) {
        for (const auto& nb : targetLevel["__neighbours"]) {
            std::string dir = nb["dir"];
            std::string levelIid = nb["levelIid"];
            for (const auto& lvl : j["levels"]) {
                if (lvl.contains("iid") && lvl["iid"] == levelIid) {
                    NeighbourInfo info;
                    info.levelName = lvl["identifier"];
                    info.worldX = lvl.contains("worldX") ? (int)lvl["worldX"] : 0;
                    info.worldY = lvl.contains("worldY") ? (int)lvl["worldY"] : 0;
                    info.width = lvl.contains("pxWid") ? (int)lvl["pxWid"] : 0;
                    info.height = lvl.contains("pxHei") ? (int)lvl["pxHei"] : 0;
                    currentNeighbours[dir].push_back(info);
                    break;
                }
            }
        }
    }


    levelWidth = targetLevel["pxWid"];
    levelHeight = targetLevel["pxHei"];
    worldX = targetLevel.contains("worldX") ? (int)targetLevel["worldX"] : 0;
    worldY = targetLevel.contains("worldY") ? (int)targetLevel["worldY"] : 0;
    
    auto& layerInstances = targetLevel["layerInstances"];
    if (!layerInstances.empty()) {
        tileSize = layerInstances[0]["__gridSize"];
        columns = layerInstances[0]["__cWid"];
        rows = layerInstances[0]["__cHei"];
    }

    collisionLayer = std::vector<std::vector<CollisionType>>(rows, std::vector<CollisionType>(columns, CollisionType::None));

    if (hasCanvas) UnloadRenderTexture(mapCanvas);

    mapCanvas = LoadRenderTexture(levelWidth, levelHeight);
    hasCanvas = true;

    BeginTextureMode(mapCanvas);
    ClearBackground(BLACK); // Nền đen mặc định cho phần không có tài nguyên/không vẽ của map
    if (hasBackgroundTexture) {
        if (targetLevel.contains("__bgPos") && !targetLevel["__bgPos"].is_null()) {
            auto bgPos = targetLevel["__bgPos"];
            auto crop = bgPos["cropRect"];
            auto scale = bgPos["scale"];
            auto topLeft = bgPos["topLeftPx"];
            Rectangle src = { (float)crop[0], (float)crop[1], (float)crop[2], (float)crop[3] };
            Rectangle dest = { (float)topLeft[0], (float)topLeft[1], src.width * (float)scale[0], src.height * (float)scale[1] };
            DrawTexturePro(backgroundTexture, src, dest, {0.0f, 0.0f}, 0.0f, WHITE);
        } else {
            DrawTexture(backgroundTexture, 0, 0, WHITE);
        }
    }

    for (int i = (int)layerInstances.size() - 1; i >= 0; --i) {
        const auto& layer = layerInstances[i];
        std::string layerName = (layer.contains("__identifier") && !layer["__identifier"].is_null()) ? (std::string)layer["__identifier"] : "";
        std::string layerType = (layer.contains("__type") && !layer["__type"].is_null()) ? (std::string)layer["__type"] : "";

        if (layerName == "Collision" || layerName == "Collisions") {
            if (layer.contains("intGridCsv") && !layer["intGridCsv"].is_null()) {
                const auto& csv = layer["intGridCsv"];
                for (size_t idx = 0; idx < csv.size(); ++idx) {
                    int val = csv[idx];
                    if (val > 0) {
                        int y = idx / columns;
                        int x = idx % columns;
                        if (y < rows && x < columns) {
                            if (intToCollisionType.find(val) != intToCollisionType.end()) {
                                collisionLayer[y][x] = intToCollisionType[val];
                            } else {
                                collisionLayer[y][x] = CollisionType::Solid;
                            }
                        }

                    }
                }
            }
            continue; // Không render lớp Collision ra màn hình
        }

        if (layerType == "Entities") {
            if (layer.contains("entityInstances") && !layer["entityInstances"].is_null()) {
                for (const auto& ent : layer["entityInstances"]) {
                    if (ent.contains("__identifier") && ent["__identifier"] == "Starting_position") {
                        playerSpawns.push_back({ (float)ent["px"][0], (float)ent["px"][1] });
                    }

                    // Collect all non-internal entities for ItemFactory
                    std::string entId = ent.contains("__identifier") ? (std::string)ent["__identifier"] : "";
                    static const std::vector<std::string> SKIP = {
                        "Starting_position", "Ladder", "BigBackground", "SmallBg",
                        "Entity", "Plant", "Teleport", "Exit"
                    };
                    bool skip = false;
                    for (auto& s : SKIP) if (entId == s) { skip = true; break; }
                    if (!skip && !entId.empty()) {
                        LDtkEntityData data;
                        data.identifier = entId;
                        
                        float pxX = ent["px"][0];
                        float pxY = ent["px"][1];
                        float pivotX = ent.contains("__pivot") && !ent["__pivot"].is_null() ? (float)ent["__pivot"][0] : 0.0f;
                        float pivotY = ent.contains("__pivot") && !ent["__pivot"].is_null() ? (float)ent["__pivot"][1] : 0.0f;
                        float w = ent.contains("width") ? (float)ent["width"] : 16.0f;
                        float h = ent.contains("height") ? (float)ent["height"] : 16.0f;
                        
                        // BaseItem expects bottom-left corner
                        data.px.x = pxX - w * pivotX;
                        data.px.y = pxY + h * (1.0f - pivotY);
                        
                        data.iid = ent.contains("iid") ? (std::string)ent["iid"] : "";
                        data.fieldInstances = ent.contains("fieldInstances") ? ent["fieldInstances"] : json::array();
                        entityData_.push_back(data);
                    }

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

std::vector<CollisionTile> TileMap::GetCollidingTiles(Rectangle entityRect) const {
    std::vector<CollisionTile> collisions;
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
            
            CollisionType tileType = collisionLayer[y][x];
            if (tileType != CollisionType::None) {
                Rectangle tileRect = { (float)(x * worldTileSize), (float)(y * worldTileSize), worldTileSize, worldTileSize };
                if (CheckCollisionRecs(entityRect, tileRect)) {
                    collisions.push_back({tileRect, tileType});
                }
            }
        }
    }
    return collisions;
}

std::string TileMap::GetNeighbour(const std::string& dir, float globalX, float globalY) const {
    auto it = currentNeighbours.find(dir);
    if (it != currentNeighbours.end()) {
        const auto& neighbours = it->second;
        if (neighbours.empty()) return "";
        
        float scale = GetWorldScale();
        for (const auto& nb : neighbours) {
            float nbWorldX = nb.worldX * scale;
            float nbWorldY = nb.worldY * scale;
            float nbWidth = nb.width * scale;
            float nbHeight = nb.height * scale;
            
            float margin = 8.0f; // Bù sai số
            
            if (dir == "e" || dir == "w") {
                if (globalY >= nbWorldY - margin && globalY <= nbWorldY + nbHeight + margin) {
                    return nb.levelName;
                }
            } else if (dir == "n" || dir == "s") {
                if (globalX >= nbWorldX - margin && globalX <= nbWorldX + nbWidth + margin) {
                    return nb.levelName;
                }
            }
        }
        return ""; // Kh\u00f4ng t\u00ecm th\u1ea5y neighbour ph\u00f9 h\u1ee3p v\u1edbi to\u1ea1 \u0111\u1ed9 hi\u1ec7n t\u1ea1i
    }
    return "";
}

std::vector<Vector2> TileMap::GetPlayerSpawns() const {
    std::vector<Vector2> scaledSpawns;
    float scale = GetWorldScale();
    for (const auto& p : playerSpawns) {
        // Return bottom-center of the 16x16 Starting_position entity
        scaledSpawns.push_back({ (p.x + 8.0f) * scale, (p.y + 16.0f) * scale });
    }
    return scaledSpawns;
}

std::vector<LDtkEntityData> TileMap::GetEntityData() const {
    // Scale all positions from LDtk tile space to game pixel space
    std::vector<LDtkEntityData> scaled;
    float scale = GetWorldScale();
    scaled.reserve(entityData_.size());
    for (const auto& data : entityData_) {
        LDtkEntityData d = data;
        d.px.x *= scale;
        d.px.y *= scale;
        scaled.push_back(std::move(d));
    }
    return scaled;
}
