#include "EditorBlockRegistry.h"
#include "AutoTiler.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

EditorBlockRegistry& EditorBlockRegistry::getInstance() {
    static EditorBlockRegistry instance;
    return instance;
}

void EditorBlockRegistry::add(EditorBlockDef def) {
    // Tự động thêm category vào danh sách thứ tự nếu chưa có
    if (std::find(orderedCategories_.begin(), orderedCategories_.end(), def.category)
            == orderedCategories_.end()) {
        orderedCategories_.push_back(def.category);
    }
    defs_[def.id] = std::move(def);
}

// =============================================================================
// init() — Đăng ký toàn bộ block defs từ catalog world01–06
// Quy tắc: nativeTileSize = kích thước gốc trong tileset (8 hoặc 16).
//          Engine scale lên 32px khi render trong editor và trong game.
// =============================================================================
void EditorBlockRegistry::init() {
    if (!defs_.empty()) return; // Chỉ init 1 lần
    
    AutoTiler::getInstance().loadRules("assets/maps/extracted_rules.json");

    // =========================================================================
    // World 01 — Tiles.png (grid=16, cWid=22)
    // IntGrid: {1=dirt, 2=bridge, 3=water, 4=hoa_sen1, 5=hoa_sen2, 6=hide_dirt, 7=stone}
    // =========================================================================
    add({ "WORLD01_DIRT",     "Solid",  "w01 Dirt",      CollisionType::Solid,  "assets/maps/map01/Tiles.png", {0, 8, 80, 72}, 16, {139, 90, 43, 255} });
    add({ "WORLD01_STONE",    "Solid",  "w01 Stone",     CollisionType::Solid,  "assets/maps/map01/Tiles.png", {0, 88, 80, 72}, 16, {128,128,128, 255} });
    add({ "WORLD01_BRIDGE",   "Solid",  "w01 Bridge",    CollisionType::Solid,  "assets/maps/map01/Tiles.png", {192, 160, 48, 32}, 16, {160,100, 60, 255} });
    add({ "WORLD01_HOA_SEN1", "Lotus",         "w01 Hoa Sen 1", CollisionType::Lotus,  "assets/maps/map01/Tiles.png", {304, 304, 32, 32}, 16, { 60,180, 60, 255} });
    add({ "WORLD01_HOA_SEN2", "Lotus",         "w01 Hoa Sen 2", CollisionType::Lotus,  "assets/maps/map01/Tiles.png", {272, 304, 32, 32}, 16, { 80,200, 80, 255} });
    add({ "WORLD01_WATER",    "Special Block", "w01 Water",     CollisionType::Water,  "assets/maps/map01/Tiles.png", {128, 296, 32, 32}, 16, { 30,100,200, 255} });

    // =========================================================================
    // World 02 — Cavernas (grid=8, cWid=12)
    // IntGrid: {1=walls}
    // =========================================================================
    add({ "WORLD02_WALLS", "Solid", "w02 Walls", CollisionType::Solid, "assets/maps/map02/Cavernas_by_Adam_Saltsman.png", {24, 32, 24, 24}, 8, {80,80,100, 255} });

    add({ "WORLD03_DIRT",   "Solid",  "w03 Dirt",   CollisionType::Solid,  "assets/maps/map03/SunnyLand_by_Ansimuz-extended.png", {0, 0, 16, 16}, 16, {139, 90, 43, 255} });
    add({ "WORLD03_STONE",  "Solid",  "w03 Stone",  CollisionType::Solid,  "assets/maps/map03/SunnyLand_by_Ansimuz-extended.png", {256, 96, 16, 16}, 16, {128,128,128, 255} });
    add({ "WORLD03_LADDER", "Ladder", "w03 Ladder", CollisionType::Ladder, "assets/maps/map03/SunnyLand_by_Ansimuz-extended.png", {96, 144, 16, 16}, 16, {200,150, 50, 255} });

    // =========================================================================
    // World 04 — NuclearBlaze (grid=16, cWid=36)
    // IntGrid: {1=walls, 2=water}
    // =========================================================================
    add({ "WORLD04_WALLS", "Solid", "w04 Walls", CollisionType::Solid, "assets/maps/map04/NuclearBlaze_by_deepnight.png", {0, 120, 48, 40}, 16, {80, 60, 60, 255} });

    // =========================================================================
    // World 05 — Multiple tilesets
    // IntGrid1: {1=dirt_grass, 2=dirt, 3=tree, 4=poison, 5=leaf1, 6=leaf2, 7=Vine, 8=wood, 9=fence}
    // IntGrid2: {1=dirt_cam, 2=dirt_xanh, 3=oneway, 4=dirt_fly}
    // IntGrid3: {1=cloud}
    // IntGrid4: {1=grass}  — GrassTileset.png
    // IntGrid5: {1=lava}
    // IntGrid6: {1=vence}  — Vines.png
    // IntGrid7: {1=water}
    // =========================================================================
    // IntGrid1 — Assets.png (cWid=25)
    add({ "WORLD05_DIRT_GRASS", "Solid",         "w05 Dirt Grass", CollisionType::Solid,  "assets/maps/map05/Assets.png", {32, 0, 48, 40}, 16, { 60,160, 40, 255} });
    add({ "WORLD05_DIRT",       "Solid",         "w05 Dirt",       CollisionType::Solid,  "assets/maps/map05/Assets.png", {16, 160, 48, 40}, 16, {139, 90, 43, 255} });
    add({ "WORLD05_TREE",       "Solid",         "w05 Tree",       CollisionType::Solid,  "assets/maps/map05/Assets.png", {144, 272, 32, 32}, 16, { 50,120, 30, 255} });
    add({ "WORLD05_POISON",     "Special Block", "w05 Poison",     CollisionType::Poison, "assets/maps/map05/Assets.png", {336, 80, 48, 48}, 16, {120,  0,180, 255} });
    add({ "WORLD05_LEAF1",      "Solid",         "w05 Leaf 1",     CollisionType::Solid,  "assets/maps/map05/Assets.png", {24, 248, 64, 64}, 16, { 80,200, 60, 255} });
    add({ "WORLD05_LEAF2",      "Solid",         "w05 Leaf 2",     CollisionType::Solid,  "assets/maps/map05/Assets.png", {72, 312, 64, 64}, 16, { 60,180, 40, 255} });
    add({ "WORLD05_VINE",       "OneWay",        "w05 Vine",       CollisionType::OneWay, "assets/maps/map05/Assets.png", {336, 384, 16,16}, 16, { 30,140, 20, 255} });
    add({ "WORLD05_WOOD",       "None",          "w05 Wood",       CollisionType::None,   "assets/maps/map05/Assets.png", {112, 128, 40, 40}, 16, {140, 90, 30, 255} });
    add({ "WORLD05_FENCE",      "Solid",         "w05 Fence",      CollisionType::Solid,  "assets/maps/map05/Assets.png", {144, 48, 64, 48}, 16, {160,120, 60, 255} });
    // IntGrid2 — Terrain_and_Props.png (cWid=20)
    add({ "WORLD05_DIRT_CAM",   "Solid",         "w05 Dirt Cam",     CollisionType::Solid,  "assets/maps/map05/Terrain_and_Props.png", {72, 8, 96, 104}, 16, {120, 80, 40, 255} });
    add({ "WORLD05_DIRT_XANH",  "Solid",         "w05 Dirt Xanh",    CollisionType::Solid,  "assets/maps/map05/Terrain_and_Props.png", {200, 8, 96, 104}, 16, { 60,160, 80, 255} });
    add({ "WORLD05_ONEWAY",     "OneWay",        "w05 WOODEN STRIP", CollisionType::OneWay, "assets/maps/map05/Terrain_and_Props.png", {80, 160, 80, 16}, 16, {200,200,100, 255} });
    add({ "WORLD05_DIRT_FLY",   "OneWay",        "w05 Dirt Fly",     CollisionType::OneWay, "assets/maps/map05/Terrain_and_Props.png", {192, 144, 96, 32}, 16, {100, 70, 30, 255} });
    // IntGrid3 — cloud tiles.png (cWid=8)
    add({ "WORLD05_CLOUD",      "Special Block", "w05 Cloud",        CollisionType::Cloud,  "assets/maps/map05/cloud tiles.png", {96, 32, 32, 32}, 16, {200,220,255, 255} });
    // IntGrid4 — GrassTileset.png (cWid=9)
    add({ "WORLD05_GRASS",      "Solid",         "w05 Leaf 3",       CollisionType::Solid,  "assets/maps/map05/GrassTileset.png", {40, 8, 40, 40}, 16, { 80,180, 50, 255} });
    // IntGrid5 — Lava (cWid=3)
    add({ "WORLD05_LAVA",       "Special Block", "w05 Lava",         CollisionType::Lava,   "assets/maps/map05/Lava_29_JE2_BE1.png", {0, 8, 32, 32}, 16, {220, 80, 10, 255} });
    // IntGrid6 — Vines.png (cWid=1)
    add({ "WORLD05_VENCE",      "Ladder",        "w05 Vence",        CollisionType::Ladder, "assets/maps/map05/Vines.png", {0, 0, 16,16}, 16, { 20,120, 20, 255} });

    std::cout << "[EditorBlockRegistry] Initialized " << defs_.size() << " block defs.\n";
}

// =============================================================================
// Accessors
// =============================================================================

const EditorBlockDef& EditorBlockRegistry::get(const std::string& blockId) const {
    auto it = defs_.find(blockId);
    if (it == defs_.end()) {
        throw std::out_of_range("[EditorBlockRegistry] Unknown blockId: " + blockId);
    }
    return it->second;
}

bool EditorBlockRegistry::has(const std::string& blockId) const {
    return defs_.count(blockId) > 0;
}

CollisionType EditorBlockRegistry::getCollision(const std::string& blockId) const {
    if (blockId.empty()) return CollisionType::None;
    return get(blockId).collision;
}

std::vector<std::string> EditorBlockRegistry::getAllInCategory(const std::string& category) const {
    std::vector<std::string> result;
    for (const auto& [id, def] : defs_) {
        if (def.category == category) {
            result.push_back(id);
        }
    }
    // Sắp xếp theo thứ tự đăng ký (displayName alphabetically)
    std::sort(result.begin(), result.end(), [&](const std::string& a, const std::string& b) {
        return defs_.at(a).displayName < defs_.at(b).displayName;
    });
    return result;
}

std::vector<std::string> EditorBlockRegistry::getOrderedCategories() const {
    return orderedCategories_;
}
