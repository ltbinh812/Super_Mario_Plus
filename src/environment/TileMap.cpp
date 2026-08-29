#include "TileMap.h"
#include "CustomMapData.h"
#include "EditorBlockRegistry.h"
#include "AutoTiler.h"
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
                        else if (id == "Vine") type = CollisionType::Vine;
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
        // Find the Collision layer to define the global physical grid
        bool foundCollisionGrid = false;
        for (const auto& layer : layerInstances) {
            std::string lName = layer.contains("__identifier") && !layer["__identifier"].is_null() ? (std::string)layer["__identifier"] : "";
            if (lName == "Collision" || lName == "Collisions") {
                tileSize = layer["__gridSize"];
                columns = layer["__cWid"];
                rows = layer["__cHei"];
                foundCollisionGrid = true;
                break;
            }
        }
        
        if (!foundCollisionGrid) {
            // Fallback to the layer that has gridTiles or autoLayerTiles
            for (const auto& layer : layerInstances) {
                if ((layer.contains("autoLayerTiles") && !layer["autoLayerTiles"].is_null() && !layer["autoLayerTiles"].empty()) ||
                    (layer.contains("gridTiles") && !layer["gridTiles"].is_null() && !layer["gridTiles"].empty()) ||
                    (layer.contains("intGridCsv") && !layer["intGridCsv"].is_null() && !layer["intGridCsv"].empty())) {
                    tileSize = layer["__gridSize"];
                    columns = layer["__cWid"];
                    rows = layer["__cHei"];
                    foundCollisionGrid = true;
                    break;
                }
            }
        }
        
        if (!foundCollisionGrid) {
            tileSize = layerInstances[0]["__gridSize"];
            columns = layerInstances[0]["__cWid"];
            rows = layerInstances[0]["__cHei"];
        }
    }

    collisionLayer = std::vector<std::vector<CollisionType>>(rows, std::vector<CollisionType>(columns, CollisionType::None));

    if (hasCanvas) { UnloadRenderTexture(mapCanvas); }
    mapCanvas = LoadRenderTexture(levelWidth, levelHeight);
    canvasW = (float)levelWidth;
    canvasH = (float)levelHeight;
    isCanvasPreScaled = false;
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
            int layerGridSize = layer.contains("__gridSize") ? (int)layer["__gridSize"] : tileSize;
            for (const auto& tile : tiles) {
                int px = tile["px"][0];
                int py = tile["px"][1];
                int srcX = tile["src"][0];
                int srcY = tile["src"][1];
                int f = tile["f"];

                Rectangle srcRect = { (float)srcX, (float)srcY, (float)layerGridSize, (float)layerGridSize };
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
        float drawScale = isCanvasPreScaled ? 1.0f : GetWorldScale();
        // Dùng đúng canvasW, canvasH đã lưu lúc tạo
        Rectangle src = { 0.0f, 0.0f, canvasW, -canvasH };
        // Tùy theo canvas đã scale chưa mà ta vẽ to ra (cho standard) hoặc giữ nguyên (cho custom)
        Rectangle dest = { 0.0f, 0.0f, canvasW * drawScale, canvasH * drawScale };
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

// =============================================================================
// [NEW] LoadCustomMap — load từ in-game editor, không cần file LDtk
//
// Chiến lược:
//  1. Điền collisionLayer từ EditorBlockRegistry (tương đương layer "Collisions" của LDtk)
//  2. Điền entityData_ và playerSpawns (tương đương entity layer của LDtk)
//  3. Load tileset textures (lazy, dùng lại tilesetTextures map đã có)
//  4. Vẽ tile visuals vào mapCanvas (dùng DrawTexturePro để scale 8/16px → 32px)
//
// Sau khi gọi xong, BaseLevelState dùng:
//  - map.GetCollidingTiles()   ← dùng collisionLayer  (không đổi)
//  - map.GetPlayerSpawns()     ← dùng playerSpawns    (không đổi)
//  - map.GetEntityData()       ← dùng entityData_     (không đổi)
//  - map.Draw()                ← dùng mapCanvas       (không đổi)
// =============================================================================
bool TileMap::LoadCustomMap(const CustomMapData& data) {
    // --- Reset state ---
    currentLevelName   = data.name;
    tileSize           = data.tileSize;   // 16 (native LDtk size)
    columns            = data.width;
    rows               = data.height;
    levelWidth         = data.width  * tileSize;
    levelHeight        = data.height * tileSize;
    worldX             = 0;
    worldY             = 0;
    currentNeighbours.clear();   // single-level editor, không có neighbours

    // --- 1. Build collision layer ---
    collisionLayer.assign(rows, std::vector<CollisionType>(columns, CollisionType::None));
    auto& reg = EditorBlockRegistry::getInstance();
    for (const auto& [key, blockId] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        if (gx >= 0 && gx < columns && gy >= 0 && gy < rows) {
            collisionLayer[gy][gx] = reg.getCollision(blockId);
        }
    }

    // --- 2. Build entity data & player spawns ---
    entityData_.clear();
    playerSpawns.clear();
    for (const auto& e : data.entities) {
        if (e.type == "PlayerSpawn") {
            // PlayerSpawn → pixel position (top-left của ô grid)
            playerSpawns.push_back({
                (float)(e.gridX * tileSize),
                (float)(e.gridY * tileSize)
            });
        } else {
            LDtkEntityData ldtk;
            ldtk.identifier    = e.type;
            ldtk.px            = { (float)(e.gridX * tileSize), (float)(e.gridY * tileSize) };
            ldtk.iid           = "custom_" + std::to_string(e.gridX) + "_" + std::to_string(e.gridY);
            ldtk.fieldInstances = e.fields.is_null() ? nlohmann::json::array() : e.fields;
            entityData_.push_back(ldtk);
        }
    }

    // --- 3. Lazy-load tileset textures ---
    // Key = tilesetPath string, value = Texture2D
    // Dùng lại tilesetTextures (unordered_map<int, Texture2D>) nhưng cần map string → int uid
    // Giải pháp: dùng hash của path string làm uid (đủ collision-free cho số lượng nhỏ)
    std::unordered_map<std::string, int> pathToUid;
    std::unordered_map<int, std::string> uidToPath;

    auto getOrLoadTexture = [&](const std::string& path) -> int {
        if (path.empty()) return -1;
        auto it = pathToUid.find(path);
        if (it != pathToUid.end()) return it->second;

        int uid = (int)std::hash<std::string>{}(path) & 0x7FFFFFFF;  // positive int
        if (tilesetTextures.find(uid) == tilesetTextures.end()) {
            Texture2D tex = LoadTexture(path.c_str());
            if (tex.id == 0) {
                std::cerr << "[LoadCustomMap] Cannot load texture: " << path << "\n";
                pathToUid[path] = -1;
                return -1;
            }
            tilesetTextures[uid] = tex;
            uidToPath[uid] = path;
            std::cout << "[LoadCustomMap] Loaded texture: " << path << "\n";
        }
        pathToUid[path] = uid;
        return uid;
    };

    // --- 4. Render mapCanvas ---
    float scale = GetWorldScale();   // tileSize < 32 → scale = 32/tileSize (thường = 2.0)
    int cw = (int)(levelWidth  * scale);
    int ch = (int)(levelHeight * scale);

    if (hasCanvas) { UnloadRenderTexture(mapCanvas); hasCanvas = false; }
    mapCanvas  = LoadRenderTexture(cw, ch);
    canvasW = (float)cw;
    canvasH = (float)ch;
    isCanvasPreScaled = true;
    hasCanvas  = true;

    BeginTextureMode(mapCanvas);
    ClearBackground(BLANK);

    float destTileSize = (float)tileSize * scale;  // = 32px nếu native=16

    // --- Pass 1: Vẽ fallback màu sắc cho các ô KHÔNG CÓ texture
    for (const auto& [key, blockId] : data.tiles) {
        if (!reg.has(blockId)) continue;
        const auto& def = reg.get(blockId);

        // Nếu block có texture (qua rules hoặc trực tiếp def.tilesetPath), KHÔNG VẼ fallback để tránh bị lộ màu dưới vùng viền trong suốt
        if (AutoTiler::getInstance().hasRulesForBlock(blockId) || !def.tilesetPath.empty()) continue;

        int gx = key % data.width;
        int gy = key / data.width;
        float destX = (float)(gx * tileSize) * scale;
        float destY = (float)(gy * tileSize) * scale;
        Rectangle dest = { destX, destY, destTileSize, destTileSize };
        DrawRectangleRec(dest, def.fallbackColor);
    }

    // --- Hàm lấy độ ưu tiên Z-Index cho layer (nhỏ = vẽ trước/nằm dưới)
    auto getLayerPriority = [](const std::string& layerId) -> int {
        if (layerId == "Bg_textures") return 10;
        if (layerId == "IntGrid7_water") return 20;
        if (layerId == "IntGrid6_vence") return 30;
        if (layerId == "Wall_shadows") return 40;
        if (layerId == "IntGrid5_lava") return 50;
        if (layerId == "IntGrid4_leaf") return 60;
        if (layerId == "IntGrid3_cloud") return 70;
        if (layerId == "IntGrid2") return 80;
        return 100; // Foreground chính (IntGrid, IntGridd, IntGrid1, IntGrid_layer)
    };

    // --- Pass 2: Full Auto-Layer scan — giống cơ chế LDtk gốc
    std::unordered_set<int> autoTiledCells;
    auto autoOutputs = AutoTiler::getInstance().buildLayer(data, autoTiledCells);
    
    // Sắp xếp lại danh sách các output theo Z-Index toàn cục
    std::stable_sort(autoOutputs.begin(), autoOutputs.end(), [&](const AutoTileOutput& a, const AutoTileOutput& b) {
        int pa = getLayerPriority(a.groupLayerId);
        int pb = getLayerPriority(b.groupLayerId);
        if (pa != pb) return pa < pb; // Khác Layer: Vẽ theo priority (nhỏ vẽ trước)
        
        // Cùng Layer: LDtk vẽ Group 0 đè lên (nằm trên) Group N
        // Nghĩa là Group N phải được vẽ TRƯỚC Group 0
        if (a.groupIndex != b.groupIndex) return a.groupIndex > b.groupIndex;
        
        // Cùng Group: LDtk vẽ Rule 0 đè lên Rule M
        return a.ruleIndex > b.ruleIndex;
    });

    for (const auto& out : autoOutputs) {
        if (out.tilesetPath.empty()) continue;
        int uid = getOrLoadTexture(out.tilesetPath);
        if (uid < 0 || !tilesetTextures.count(uid)) continue;
        Texture2D tex = tilesetTextures.at(uid);

        // Thay vì dùng chung scale của toàn map, ta tính scale RIÊNG cho từng viên gạch.
        // Mục tiêu: Gạch (dù 8 hay 16 gốc) đều phóng to lấp đầy 1 ô vật lý 32x32 pixel!
        float tileScale = 32.0f / (float)out.tileSize; 
        
        float destX = out.px * tileScale;
        float destY = out.py * tileScale;

        // Cờ lật (f): 0=None, 1=FlipX, 2=FlipY, 3=FlipX & FlipY
        Rectangle src = out.uv;
        if (out.f & 1) src.width = -src.width;
        if (out.f & 2) src.height = -src.height;

        // Đảm bảo dest có width/height dương, nhưng để draw chính xác khi có lật ảnh thì 
        // dest.width và dest.height phải dương, còn lật ảnh thì src.width/height mang dấu âm.
        float destWidth = std::abs(src.width) * tileScale;
        float destHeight = std::abs(src.height) * tileScale;
        Rectangle dest = { destX, destY, destWidth, destHeight };

        DrawTexturePro(tex, src, dest, {0, 0}, 0.0f, WHITE);
    }

    // --- Pass 3: Vẽ fallback cho blocks không có rule group hoặc không match rule nào
    for (const auto& [key, blockId] : data.tiles) {
        if (!reg.has(blockId)) continue;
        
        int gx = key % data.width;
        int gy = key / data.width;
        
        // Nếu block này ĐÃ tự động sinh ra được tile của chính nó thông qua AutoTiler, thì không cần vẽ fallback.
        if (autoTiledCells.count(gy * data.width + gx)) continue;
        
        // Nếu nó CÓ khai báo trong rules nhưng bị thiếu rule (e.g. đặt block lẻ) thì nó sẽ không có trong autoTiledCells
        // nên nó sẽ vẽ fallback! (Đây chính là lý do World 01 dirt bị mất tích nếu bị "bóp" thành rules rỗng)

        const auto& def = reg.get(blockId);
        if (def.tilesetPath.empty()) continue;

        int uid = getOrLoadTexture(def.tilesetPath);
        if (uid < 0 || !tilesetTextures.count(uid)) continue;
        Texture2D tex = tilesetTextures.at(uid);

        float destX = (float)(gx * tileSize) * scale;
        float destY = (float)(gy * tileSize) * scale;
        
        float scaleX = destTileSize / std::abs(def.uv.width);
        float scaleY = destTileSize / std::abs(def.uv.height);
        float minScale = std::min(scaleX, scaleY);
        
        float newW = std::abs(def.uv.width) * minScale;
        float newH = std::abs(def.uv.height) * minScale;
        float offX = (destTileSize - newW) / 2.0f;
        float offY = (destTileSize - newH) / 2.0f;
        
        Rectangle dest = { destX + offX, destY + offY, newW, newH };
        DrawTexturePro(tex, def.uv, dest, {0, 0}, 0.0f, WHITE);
    }

    EndTextureMode();

    std::cout << "[TileMap] LoadCustomMap: " << data.name
              << " (" << columns << "x" << rows << ") OK.\n";
    return true;
}
