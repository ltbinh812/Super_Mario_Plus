#include "CustomMapSerializer.h"
#include "EditorBlockRegistry.h"
#include "EditorTextureCache.h"
#include "AutoTiler.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using json   = nlohmann::json;

// =============================================================================
// Helpers
// =============================================================================

std::string CustomMapSerializer::slotPath(int slot) {
    return "saves/custom_map_" + std::to_string(slot) + ".json";
}

bool CustomMapSerializer::slotExists(int slot) {
    return fs::exists(slotPath(slot));
}

std::vector<int> CustomMapSerializer::listOccupiedSlots() {
    std::vector<int> result;
    for (int i = 0; i < MAX_SLOTS; ++i) {
        if (slotExists(i)) result.push_back(i);
    }
    return result;
}

std::string CustomMapSerializer::getSlotName(int slot) {
    if (!slotExists(slot)) return "Empty";
    try {
        std::ifstream f(slotPath(slot));
        json j; f >> j;
        if (j.contains("name")) return j["name"].get<std::string>();
    } catch (...) {}
    return "Slot " + std::to_string(slot);
}

// =============================================================================
// Save
// Format JSON:
// {
//   "version": 1, "name": "My Map", "width": 20, "height": 15, "tileSize": 16,
//   "tiles": [ {"key": 380, "blockId": "DIRT_SUNNY"}, ... ],
//   "entities": [ {"type": "PlayerSpawn", "gridX": 2, "gridY": 13, "fields": {}}, ... ]
// }
// =============================================================================

bool CustomMapSerializer::save(const CustomMapData& data, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) {
        std::cerr << "[CustomMapSerializer] Invalid slot: " << slot << "\n";
        return false;
    }

    try {
        fs::create_directories("saves");

        json j;
        j["version"]  = data.version;
        j["name"]     = data.name;
        j["width"]    = data.width;
        j["height"]   = data.height;
        j["tileSize"] = data.tileSize;

        // Sparse tiles: chỉ lưu ô khác EMPTY
        json tilesArr = json::array();
        for (const auto& [key, blockId] : data.tiles) {
            tilesArr.push_back({ {"key", key}, {"blockId", blockId} });
        }
        j["tiles"] = tilesArr;

        // Entities
        json entArr = json::array();
        for (const auto& e : data.entities) {
            json entry;
            entry["type"]   = e.type;
            entry["gridX"]  = e.gridX;
            entry["gridY"]  = e.gridY;
            entry["fields"] = e.fields.is_null() ? json::object() : e.fields;
            entArr.push_back(entry);
        }
        j["entities"] = entArr;

        std::ofstream f(slotPath(slot));
        f << j.dump(2);  // indented 2 spaces — dễ đọc khi debug
        std::cout << "[CustomMapSerializer] Saved to slot " << slot
                  << " (" << slotPath(slot) << ")\n";
                  
        // Also export to LDtk
        exportToLDtk(data, slot);
        
        return true;

    } catch (const std::exception& ex) {
        std::cerr << "[CustomMapSerializer] Save failed: " << ex.what() << "\n";
        return false;
    }
}

// =============================================================================
// Load
// =============================================================================

CustomMapData CustomMapSerializer::load(int slot) {
    CustomMapData data;
    if (!slotExists(slot)) {
        std::cerr << "[CustomMapSerializer] Slot " << slot << " does not exist.\n";
        return data;
    }

    try {
        std::ifstream f(slotPath(slot));
        json j; f >> j;

        data.version  = j.value("version",  1);
        data.name     = j.value("name",     "My Map");
        data.width    = j.value("width",    20);
        data.height   = j.value("height",   15);
        data.tileSize = j.value("tileSize", 16);

        // Tiles
        data.tiles.clear();
        if (j.contains("tiles") && j["tiles"].is_array()) {
            for (const auto& entry : j["tiles"]) {
                int         key     = entry["key"].get<int>();
                std::string blockId = entry["blockId"].get<std::string>();
                data.tiles[key] = blockId;
            }
        }

        // Entities
        data.entities.clear();
        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entry : j["entities"]) {
                CustomEntityData e;
                e.type   = entry["type"].get<std::string>();
                e.gridX  = entry["gridX"].get<int>();
                e.gridY  = entry["gridY"].get<int>();
                e.fields = entry.value("fields", json::object());
                data.entities.push_back(e);
            }
        }

        std::cout << "[CustomMapSerializer] Loaded slot " << slot
                  << " — " << data.name
                  << " (" << data.width << "x" << data.height << ")\n";

    } catch (const std::exception& ex) {
        std::cerr << "[CustomMapSerializer] Load failed: " << ex.what() << "\n";
    }

    return data;
}

// =============================================================================
// Export to LDtk
// =============================================================================

bool CustomMapSerializer::exportToLDtk(const CustomMapData& data, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return false;
    try {
        fs::create_directories("saves");
        std::string ldtkPath = "saves/custom_map_" + std::to_string(slot) + ".ldtk";

        json root;
        root["iid"] = "custom_map_" + std::to_string(slot);
        root["jsonVersion"] = "1.5.3";
        root["defaultPivotX"] = 0.0;
        root["defaultPivotY"] = 0.0;
        root["defaultGridSize"] = data.tileSize;
        root["bgColor"] = "#000000";
        root["defaultLevelBgColor"] = "#000000";
        root["externalLevels"] = false;
        root["appBuildId"] = 473703;
        root["__header__"] = {
            {"fileType", "LDtk Project JSON"},
            {"app", "LDtk"},
            {"doc", "https://ldtk.io/json"},
            {"schema", "https://ldtk.io/files/JSON_SCHEMA.json"},
            {"appAuthor", "Sebastien 'deepnight' Benard"},
            {"appVersion", "1.5.3"},
            {"url", "https://ldtk.io"}
        };
        root["toc"] = json::array();
        root["customCommands"] = json::array();
        root["flags"] = json::array();
        root["worlds"] = json::array();
        root["worldLayout"] = "LinearHorizontal";
        root["worldGridWidth"] = 256;
        root["worldGridHeight"] = 256;
        root["defaultLevelWidth"] = 256;
        root["defaultLevelHeight"] = 256;
        root["defaultEntityWidth"] = 16;
        root["defaultEntityHeight"] = 16;
        root["backupLimit"] = 10;
        root["nextUid"] = 1000;
        root["backupOnSave"] = false;
        root["exportLevelBg"] = true;
        root["exportTiled"] = false;
        root["identifierStyle"] = "Capitalize";
        root["imageExportMode"] = "None";
        root["levelNamePattern"] = "Level_%idx";
        root["minifyJson"] = false;
        root["simplifiedExport"] = false;
        root["dummyWorldIid"] = "dummy_world_" + std::to_string(slot);

        // 1. Gather all required tilesets
        auto& reg = EditorBlockRegistry::getInstance();
        std::unordered_map<std::string, int> pathToUid;
        std::vector<std::string> uniquePaths;
        
        for (const auto& [key, blockId] : data.tiles) {
            if (reg.has(blockId)) {
                std::string path = reg.get(blockId).tilesetPath;
                if (!path.empty() && pathToUid.find(path) == pathToUid.end()) {
                    pathToUid[path] = pathToUid.size() + 1;
                    uniquePaths.push_back(path);
                }
            }
        }

        std::unordered_set<int> autoTiledCells;
        auto autoOutputs = AutoTiler::getInstance().buildLayer(data, autoTiledCells);
        
        std::unordered_map<std::string, std::vector<AutoTileOutput>> autoOutputByTileset;
        for (const auto& out : autoOutputs) {
            autoOutputByTileset[out.tilesetPath].push_back(out);
            if (pathToUid.find(out.tilesetPath) == pathToUid.end()) {
                pathToUid[out.tilesetPath] = pathToUid.size() + 1;
                uniquePaths.push_back(out.tilesetPath);
            }
        }

        // 2. Build defs
        json defs = json::object();
        json tilesets = json::array();
        for (const auto& path : uniquePaths) {
            EditorTextureCache::getInstance().ensure(path);
            const Texture2D& tex = EditorTextureCache::getInstance().getOrDefault(path);
            int w = tex.id != 0 ? tex.width : 1024;
            int h = tex.id != 0 ? tex.height : 1024;

            json ts;
            ts["identifier"] = "Tileset_" + std::to_string(pathToUid[path]);
            ts["uid"] = pathToUid[path];
            ts["relPath"] = "../" + path;
            ts["pxWid"] = w;
            ts["pxHei"] = h;
            ts["__cWid"] = w / data.tileSize;
            ts["__cHei"] = h / data.tileSize;
            ts["tileGridSize"] = data.tileSize;
            ts["spacing"] = 0;
            ts["padding"] = 0;
            ts["customData"] = json::array();
            ts["enumTags"] = json::array();
            ts["savedSelections"] = json::array();
            ts["tags"] = json::array();
            tilesets.push_back(ts);
        }
        defs["tilesets"] = tilesets;

        json layers = json::array();
        // Collision IntGrid Layer Def
        json collLayerDef;
        collLayerDef["__type"] = "IntGrid";
        collLayerDef["type"] = "IntGrid";
        collLayerDef["identifier"] = "Collisions";
        collLayerDef["uid"] = 100;
        collLayerDef["gridSize"] = data.tileSize;
        collLayerDef["displayOpacity"] = 1.0;
        collLayerDef["inactiveOpacity"] = 1.0;
        collLayerDef["parallaxFactorX"] = 0.0;
        collLayerDef["parallaxFactorY"] = 0.0;
        collLayerDef["pxOffsetX"] = 0;
        collLayerDef["pxOffsetY"] = 0;
        collLayerDef["tilePivotX"] = 0.0;
        collLayerDef["tilePivotY"] = 0.0;
        collLayerDef["guideGridWid"] = 0;
        collLayerDef["guideGridHei"] = 0;
        collLayerDef["canSelectWhenInactive"] = true;
        collLayerDef["hideFieldsWhenInactive"] = true;
        collLayerDef["hideInList"] = false;
        collLayerDef["parallaxScaling"] = true;
        collLayerDef["renderInWorldView"] = true;
        collLayerDef["useAsyncRender"] = false;
        json intGridValues = json::array();
        intGridValues.push_back({{"value", 1}, {"identifier", "Solid"}, {"color", "#000000"}});
        intGridValues.push_back({{"value", 2}, {"identifier", "OneWay"}, {"color", "#555555"}});
        intGridValues.push_back({{"value", 3}, {"identifier", "Hazard"}, {"color", "#FF0000"}});
        intGridValues.push_back({{"value", 4}, {"identifier", "Ladder"}, {"color", "#8B4513"}});
        intGridValues.push_back({{"value", 6}, {"identifier", "Water"}, {"color", "#0000FF"}});
        intGridValues.push_back({{"value", 7}, {"identifier", "Lotus"}, {"color", "#FF69B4"}});
        intGridValues.push_back({{"value", 8}, {"identifier", "Cloud"}, {"color", "#FFFFFF"}});
        intGridValues.push_back({{"value", 9}, {"identifier", "Poison"}, {"color", "#800080"}});
        intGridValues.push_back({{"value", 10}, {"identifier", "Lava"}, {"color", "#FF4500"}});
        intGridValues.push_back({{"value", 11}, {"identifier", "Slope"}, {"color", "#A9A9A9"}});
        intGridValues.push_back({{"value", 12}, {"identifier", "Vine"}, {"color", "#228B22"}});
        intGridValues.push_back({{"value", 13}, {"identifier", "Die"}, {"color", "#000000"}});
        collLayerDef["intGridValues"] = intGridValues;
        collLayerDef["autoRuleGroups"] = json::array();
        collLayerDef["excludedTags"] = json::array();
        collLayerDef["intGridValuesGroups"] = json::array();
        collLayerDef["requiredTags"] = json::array();
        collLayerDef["uiFilterTags"] = json::array();
        layers.push_back(collLayerDef);
        
        // Entity Layer Def
        json entLayerDef;
        entLayerDef["__type"] = "Entities";
        entLayerDef["type"] = "Entities";
        entLayerDef["identifier"] = "Entities";
        entLayerDef["uid"] = 101;
        entLayerDef["gridSize"] = data.tileSize;
        entLayerDef["displayOpacity"] = 1.0;
        entLayerDef["inactiveOpacity"] = 1.0;
        entLayerDef["parallaxFactorX"] = 0.0;
        entLayerDef["parallaxFactorY"] = 0.0;
        entLayerDef["pxOffsetX"] = 0;
        entLayerDef["pxOffsetY"] = 0;
        entLayerDef["tilePivotX"] = 0.0;
        entLayerDef["tilePivotY"] = 0.0;
        entLayerDef["guideGridWid"] = 0;
        entLayerDef["guideGridHei"] = 0;
        entLayerDef["canSelectWhenInactive"] = true;
        entLayerDef["hideFieldsWhenInactive"] = true;
        entLayerDef["hideInList"] = false;
        entLayerDef["parallaxScaling"] = true;
        entLayerDef["renderInWorldView"] = true;
        entLayerDef["useAsyncRender"] = false;
        entLayerDef["autoRuleGroups"] = json::array();
        entLayerDef["excludedTags"] = json::array();
        entLayerDef["intGridValuesGroups"] = json::array();
        entLayerDef["requiredTags"] = json::array();
        entLayerDef["uiFilterTags"] = json::array();
        layers.push_back(entLayerDef);

        for (const auto& path : uniquePaths) {
            int uid = pathToUid[path];
            
            // AutoLayer Def
            json autoDef;
            autoDef["__type"] = "IntGrid";
            autoDef["type"] = "IntGrid";
            autoDef["identifier"] = "AutoLayer_" + std::to_string(uid);
            autoDef["uid"] = 200 + uid;
            autoDef["gridSize"] = data.tileSize;
            autoDef["displayOpacity"] = 1.0;
            autoDef["inactiveOpacity"] = 1.0;
            autoDef["parallaxFactorX"] = 0.0;
            autoDef["parallaxFactorY"] = 0.0;
            autoDef["pxOffsetX"] = 0;
            autoDef["pxOffsetY"] = 0;
            autoDef["tilePivotX"] = 0.0;
            autoDef["tilePivotY"] = 0.0;
            autoDef["guideGridWid"] = 0;
            autoDef["guideGridHei"] = 0;
            autoDef["canSelectWhenInactive"] = true;
            autoDef["hideFieldsWhenInactive"] = true;
            autoDef["hideInList"] = false;
            autoDef["parallaxScaling"] = true;
            autoDef["renderInWorldView"] = true;
            autoDef["useAsyncRender"] = false;
            autoDef["tilesetDefUid"] = uid;
            autoDef["intGridValues"] = json::array(); // AutoLayer can just be an empty IntGrid
            autoDef["autoRuleGroups"] = json::array();
            autoDef["excludedTags"] = json::array();
            autoDef["intGridValuesGroups"] = json::array();
            autoDef["requiredTags"] = json::array();
            autoDef["uiFilterTags"] = json::array();
            layers.push_back(autoDef);

            // Tiles Def
            json tileDef;
            tileDef["__type"] = "Tiles";
            tileDef["type"] = "Tiles";
            tileDef["identifier"] = "Tiles_" + std::to_string(uid);
            tileDef["uid"] = 300 + uid;
            tileDef["gridSize"] = data.tileSize;
            tileDef["displayOpacity"] = 1.0;
            tileDef["inactiveOpacity"] = 1.0;
            tileDef["parallaxFactorX"] = 0.0;
            tileDef["parallaxFactorY"] = 0.0;
            tileDef["pxOffsetX"] = 0;
            tileDef["pxOffsetY"] = 0;
            tileDef["tilePivotX"] = 0.0;
            tileDef["tilePivotY"] = 0.0;
            tileDef["guideGridWid"] = 0;
            tileDef["guideGridHei"] = 0;
            tileDef["canSelectWhenInactive"] = true;
            tileDef["hideFieldsWhenInactive"] = true;
            tileDef["hideInList"] = false;
            tileDef["parallaxScaling"] = true;
            tileDef["renderInWorldView"] = true;
            tileDef["useAsyncRender"] = false;
            tileDef["tilesetDefUid"] = uid;
            tileDef["autoRuleGroups"] = json::array();
            tileDef["excludedTags"] = json::array();
            tileDef["intGridValuesGroups"] = json::array();
            tileDef["requiredTags"] = json::array();
            tileDef["uiFilterTags"] = json::array();
            layers.push_back(tileDef);
        }

        defs["layers"] = layers;
        json entities = json::array();
        std::unordered_map<std::string, int> entityTypeToUid;
        int nextEntUid = 400;

        for (const auto& e : data.entities) {
            std::string ident = (e.type == "PlayerSpawn") ? "Starting_position" : e.type;
            if (entityTypeToUid.find(ident) == entityTypeToUid.end()) {
                entityTypeToUid[ident] = nextEntUid++;
                json def;
                def["uid"] = entityTypeToUid[ident];
                def["identifier"] = ident;
                def["width"] = 16;
                def["height"] = 16;
                def["color"] = "#94D9B3";
                def["renderMode"] = "Rectangle";
                def["showName"] = true;
                def["tilesetId"] = nullptr;
                def["tileRenderMode"] = "FitInside";
                def["tileRect"] = nullptr;
                def["uiTileRect"] = nullptr;
                def["nineSliceBorders"] = json::array();
                def["maxCount"] = 0;
                def["limitScope"] = "PerLevel";
                def["limitBehavior"] = "MoveLastOne";
                def["pivotX"] = 0.0;
                def["pivotY"] = 0.0;
                def["tags"] = json::array();
                def["exportToToc"] = false;
                def["allowOutOfBounds"] = false;
                def["doc"] = nullptr;
                def["fillOpacity"] = 1.0;
                def["lineOpacity"] = 1.0;
                def["hollow"] = false;
                def["keepAspectRatio"] = false;
                def["resizableX"] = false;
                def["resizableY"] = false;
                def["minWidth"] = nullptr;
                def["maxWidth"] = nullptr;
                def["minHeight"] = nullptr;
                def["maxHeight"] = nullptr;
                def["fieldDefs"] = json::array();
                def["tileOpacity"] = 1.0;
                entities.push_back(def);
            }
        }
        defs["entities"] = entities;
        defs["enums"] = json::array();
        defs["externalEnums"] = json::array();
        defs["levelFields"] = json::array();
        root["defs"] = defs;

        // 3. Build Level
        json level;
        level["identifier"] = data.name;
        level["iid"] = "custom_level_" + std::to_string(slot);
        level["uid"] = 0;
        level["worldX"] = 0;
        level["worldY"] = 0;
        level["worldDepth"] = 0;
        level["bgPivotX"] = 0.5;
        level["bgPivotY"] = 0.5;
        level["useAutoIdentifier"] = false;
        level["__bgColor"] = "#000000";
        level["bgColor"] = "#000000";
        level["pxWid"] = data.width * data.tileSize;
        level["pxHei"] = data.height * data.tileSize;
        level["fieldInstances"] = json::array();
        level["__neighbours"] = json::array();
        
        json layerInstances = json::array();

        // 3.1 Entities Instance
        json entInst;
        entInst["__identifier"] = "Entities";
        entInst["__type"] = "Entities";
        entInst["__cWid"] = data.width;
        entInst["__cHei"] = data.height;
        entInst["__gridSize"] = data.tileSize;
        entInst["layerDefUid"] = 101;
        entInst["__opacity"] = 1.0;
        entInst["__pxTotalOffsetX"] = 0;
        entInst["__pxTotalOffsetY"] = 0;
        entInst["levelId"] = 0;
        entInst["pxOffsetX"] = 0;
        entInst["pxOffsetY"] = 0;
        entInst["seed"] = 0;
        entInst["iid"] = "layer_ent";
        entInst["visible"] = true;
        entInst["autoLayerTiles"] = json::array();
        entInst["gridTiles"] = json::array();
        entInst["intGridCsv"] = json::array();
        entInst["optionalRules"] = json::array();
        entInst["entityInstances"] = json::array();
        for (const auto& e : data.entities) {
            std::string ident = (e.type == "PlayerSpawn") ? "Starting_position" : e.type;
            json ent;
            ent["__identifier"] = ident;
            ent["px"] = { e.gridX * data.tileSize, e.gridY * data.tileSize };
            ent["fieldInstances"] = e.fields.is_array() ? e.fields : json::array();
            ent["defUid"] = entityTypeToUid[ident];
            ent["iid"] = "ent_" + std::to_string(e.gridX) + "_" + std::to_string(e.gridY);
            ent["__tags"] = json::array();
            ent["__grid"] = { e.gridX, e.gridY };
            ent["__pivot"] = { 0.0, 0.0 };
            ent["width"] = 16;
            ent["height"] = 16;
            entInst["entityInstances"].push_back(ent);
        }
        layerInstances.push_back(entInst);

        // 3.2 Collision Instance
        json colInst;
        colInst["__identifier"] = "Collisions";
        colInst["__type"] = "IntGrid";
        colInst["__cWid"] = data.width;
        colInst["__cHei"] = data.height;
        colInst["__gridSize"] = data.tileSize;
        colInst["layerDefUid"] = 100;
        colInst["__opacity"] = 1.0;
        colInst["__pxTotalOffsetX"] = 0;
        colInst["__pxTotalOffsetY"] = 0;
        colInst["levelId"] = 0;
        colInst["pxOffsetX"] = 0;
        colInst["pxOffsetY"] = 0;
        colInst["seed"] = 0;
        colInst["iid"] = "layer_col";
        colInst["visible"] = true;
        colInst["autoLayerTiles"] = json::array();
        colInst["gridTiles"] = json::array();
        colInst["entityInstances"] = json::array();
        colInst["optionalRules"] = json::array();
        colInst["intGridCsv"] = std::vector<int>(data.width * data.height, 0);

        for (const auto& [key, blockId] : data.tiles) {
            if (reg.has(blockId)) {
                CollisionType ct = reg.get(blockId).collision;
                int val = 0;
                switch (ct) {
                    case CollisionType::Solid: val = 1; break;
                    case CollisionType::OneWay: val = 2; break;
                    case CollisionType::Hazard: val = 3; break;
                    case CollisionType::Ladder: val = 4; break;
                    case CollisionType::Water: val = 6; break;
                    case CollisionType::Lotus: val = 7; break;
                    case CollisionType::Cloud: val = 8; break;
                    case CollisionType::Poison: val = 9; break;
                    case CollisionType::Lava: val = 10; break;
                    case CollisionType::Slop: val = 11; break;
                    case CollisionType::Vine: val = 12; break;
                    case CollisionType::Die: val = 13; break;
                    default: val = 0; break;
                }
                colInst["intGridCsv"][key] = val;
            }
        }
        layerInstances.push_back(colInst);

        // 3.3 AutoLayer Instances
        for (const auto& [tsPath, outputs] : autoOutputByTileset) {
            int uid = pathToUid[tsPath];
            json autoInst;
            autoInst["__identifier"] = "AutoLayer_" + std::to_string(uid);
            autoInst["__type"] = "IntGrid";
            autoInst["__cWid"] = data.width;
            autoInst["__cHei"] = data.height;
            autoInst["__gridSize"] = data.tileSize;
            autoInst["layerDefUid"] = 200 + uid;
            autoInst["__opacity"] = 1.0;
            autoInst["__pxTotalOffsetX"] = 0;
            autoInst["__pxTotalOffsetY"] = 0;
            autoInst["levelId"] = 0;
            autoInst["pxOffsetX"] = 0;
            autoInst["pxOffsetY"] = 0;
            autoInst["seed"] = 0;
            autoInst["iid"] = "layer_auto_" + std::to_string(uid);
            autoInst["visible"] = true;
            autoInst["gridTiles"] = json::array();
            autoInst["entityInstances"] = json::array();
            autoInst["intGridCsv"] = json::array();
            autoInst["__tilesetDefUid"] = uid;
            autoInst["optionalRules"] = json::array();
            autoInst["autoLayerTiles"] = json::array();

            for (const auto& out : outputs) {
                json tile;
                tile["px"] = { (int)out.px, (int)out.py };
                
                Rectangle src = out.uv;
                int f = 0;
                if (src.width < 0) { f |= 1; src.width = -src.width; }
                if (src.height < 0) { f |= 2; src.height = -src.height; }
                
                tile["src"] = { (int)src.x, (int)src.y };
                tile["f"] = f;
                tile["d"] = json::array({0});
                tile["t"] = ((int)src.y / data.tileSize) * (1024 / data.tileSize) + ((int)src.x / data.tileSize);
                tile["a"] = 1.0;
                autoInst["autoLayerTiles"].push_back(tile);
            }
            layerInstances.push_back(autoInst);
        }

        // 3.4 Tiles Instances (Manual Blocks)
        for (const auto& path : uniquePaths) {
            int uid = pathToUid[path];
            json tileInst;
            tileInst["__identifier"] = "Tiles_" + std::to_string(uid);
            tileInst["__type"] = "Tiles";
            tileInst["__cWid"] = data.width;
            tileInst["__cHei"] = data.height;
            tileInst["__gridSize"] = data.tileSize;
            tileInst["layerDefUid"] = 300 + uid;
            tileInst["__opacity"] = 1.0;
            tileInst["__pxTotalOffsetX"] = 0;
            tileInst["__pxTotalOffsetY"] = 0;
            tileInst["levelId"] = 0;
            tileInst["pxOffsetX"] = 0;
            tileInst["pxOffsetY"] = 0;
            tileInst["seed"] = 0;
            tileInst["iid"] = "layer_tile_" + std::to_string(uid);
            tileInst["visible"] = true;
            tileInst["autoLayerTiles"] = json::array();
            tileInst["entityInstances"] = json::array();
            tileInst["intGridCsv"] = json::array();
            tileInst["__tilesetDefUid"] = uid;
            tileInst["optionalRules"] = json::array();
            tileInst["gridTiles"] = json::array();

            for (const auto& [key, blockId] : data.tiles) {
                if (!reg.has(blockId)) continue;
                if (autoTiledCells.find(key) != autoTiledCells.end()) continue; // Skip auto-tiled blocks
                
                auto& def = reg.get(blockId);
                if (def.tilesetPath != path) continue;

                int gx = key % data.width;
                int gy = key / data.width;
                
                Rectangle src = def.uv;

                int f = 0;
                if (src.width < 0) { f |= 1; src.width = -src.width; }
                if (src.height < 0) { f |= 2; src.height = -src.height; }

                json tile;
                tile["px"] = { gx * data.tileSize, gy * data.tileSize };
                tile["src"] = { (int)src.x, (int)src.y };
                tile["f"] = f;
                tile["d"] = json::array({0});
                tile["t"] = ((int)src.y / data.tileSize) * (1024 / data.tileSize) + ((int)src.x / data.tileSize);
                tile["a"] = 1.0;
                tileInst["gridTiles"].push_back(tile);
            }
            layerInstances.push_back(tileInst);
        }

        level["layerInstances"] = layerInstances;
        root["levels"] = json::array({level});

        std::ofstream f(ldtkPath);
        f << root.dump(2);
        std::cout << "[CustomMapSerializer] Exported to LDtk: " << ldtkPath << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "[CustomMapSerializer] LDtk export failed: " << ex.what() << "\n";
        return false;
    }
}
