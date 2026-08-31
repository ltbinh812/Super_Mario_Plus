#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include "raylib.h"

// =============================================================================
// AutoTileRule — Một rule trong LDtk, giữ nguyên pattern integer gốc
// =============================================================================
struct AutoTileRule {
    int size;                    // 3 hoặc 5 (pattern = size*size cells)
    std::vector<int> pattern;    // raw LDtk integers:
                                 //   0        = Any (don't care)
                                 //   N > 0    = phải là IntGrid value N
                                 //  -N        = KHÔNG phải IntGrid value N
                                 //   1000001  = Any non-empty (!= 0)
                                 //  -1000001  = phải empty (== 0)
    float chance;
    bool breakOnMatch;
    int xModulo;
    int yModulo;
    std::optional<int> outOfBoundsValue;
    int f; // flip flag (0=normal, 1=flipX, 2=flipY, 3=both)
    int xOffset = 0;
    int yOffset = 0;
    std::vector<Rectangle> uvs; // Danh sách UV có thể chọn ngẫu nhiên
};

// =============================================================================
// AutoTileRuleGroup — 1 IntGrid Layer của LDtk = 1 RuleGroup
// =============================================================================
struct AutoTileRuleGroup {
    std::string blockId;            // Ví dụ: "WORLD01_DIRT"
    std::string groupLayerId;       // LDtk layer id
    int ownIntGridValue;            // IntGrid value của block này (ví dụ: 1)
    std::string tilesetPath;        // "assets/maps/map01/Tiles.png"
    int tilesetCols;                // Số cột của tileset
    int tileSize;                   // Kích thước tile gốc (thường 16)

    // Ánh xạ IntGrid value → blockId trong cùng 1 layer
    // Ví dụ: {1: "WORLD01_DIRT", 7: "WORLD01_STONE", 3: "WORLD01_WATER"}
    std::unordered_map<int, std::string> intGridToBlockId;
    // Reverse: blockId → intGrid value
    std::unordered_map<std::string, int> blockIdToIntGrid;

    // Ánh xạ group index -> danh sách các IntGrid value
    // Dùng cho pattern check (VD: 2000 + groupIndex)
    std::unordered_map<int, std::vector<int>> groupMappings;

    std::vector<AutoTileRule> rules;
};

// =============================================================================
// AutoTileOutput — Kết quả render: tọa độ + UV + tileset
// =============================================================================
struct AutoTileOutput {
    float px; // native pixel X (ví dụ 16, 32, 40...)
    float py; // native pixel Y
    Rectangle uv;
    std::string tilesetPath;
    int tileSize; // kích thước gốc của rule (8 hoặc 16)
    int f = 0;    // cờ lật (0=normal, 1=flipX, 2=flipY, 3=both)
    std::string groupLayerId; // LDtk layer name
    int groupIndex = 0; // Vị trí của group trong danh sách (dùng để sắp xếp z-index)
    int ruleIndex = 0;  // Vị trí của rule trong group
};

// =============================================================================
// AutoTiler — Singleton, thực hiện full-map scan giống LDtk engine
// =============================================================================
class AutoTiler {
public:
    static AutoTiler& getInstance() {
        static AutoTiler instance;
        return instance;
    }

    // Load rules từ extracted_rules.json (format mới: list of RuleGroup)
    void loadRules(const std::string& filepath);

    // Full-map scan: trả về danh sách tất cả tiles cần render
    // data truyền vào phải implement getTile(x,y) và inBounds(x,y)
    template<typename MapData>
    std::vector<AutoTileOutput> buildLayer(const MapData& data, std::unordered_set<int>& autoTiledCells) const;

    // Kiểm tra nhanh xem block có rule group không
    bool hasRulesForBlock(const std::string& blockId) const {
        return blockToGroupIndex_.count(blockId) > 0;
    }

    // Trả về group index của block (-1 nếu không có)
    int getGroupIndex(const std::string& blockId) const {
        auto it = blockToGroupIndex_.find(blockId);
        return it != blockToGroupIndex_.end() ? it->second : -1;
    }

    const std::vector<AutoTileRuleGroup>& getGroups() const { return groups_; }

private:
    AutoTiler() = default;

    std::vector<AutoTileRuleGroup> groups_;
    // blockId → index trong groups_ (mỗi block thuộc 1 group)
    std::unordered_map<std::string, int> blockToGroupIndex_;

    // Match pattern tại (cx, cy) với hàm lấy IntGrid value
    bool matchPattern(const AutoTileRuleGroup& group, const AutoTileRule& rule, int cx, int cy,
                      const std::function<int(int, int, std::optional<int>)>& getIntGridValue) const;
};

// =============================================================================
// Template implementation (phải ở trong header)
// =============================================================================
template<typename MapData>
std::vector<AutoTileOutput> AutoTiler::buildLayer(const MapData& data, std::unordered_set<int>& autoTiledCells) const {
    std::vector<AutoTileOutput> outputs;
    
    if (groups_.empty()) return outputs;
    
    // LDtk defines rules from top to bottom (Group 0 to N).
    // In LDtk, rules are evaluated per-cell. If a rule matches and has breakOnMatch=true,
    // it stops evaluating ALL subsequent rules for that cell (across all groups in the same layer).
    int ratio = 1;
    int virtualWidth = data.width * ratio;
    int virtualHeight = data.height * ratio;

    for (int vy = -1; vy <= virtualHeight; ++vy) {
        for (int vx = -1; vx <= virtualWidth; ++vx) {
            srand(vy * virtualWidth + vx + 12345);
            std::unordered_map<std::string, bool> layerHandled;

            // Iterate groups from Group 0 to N (Forward order)
            int groupIndex = 0;
            for (auto it = groups_.begin(); it != groups_.end(); ++it, ++groupIndex) {
                const auto& group = *it;
                if (layerHandled[group.groupLayerId]) continue; // LDtk cross-group break logic per layer
                
                
                auto getIntGridValue = [&](int vx2, int vy2, std::optional<int> oob) -> int {
                    if (vx2 < 0 || vx2 >= virtualWidth || vy2 < 0 || vy2 >= virtualHeight) {
                        return oob.has_value() ? oob.value() : 0;
                    }
                    int gx = vx2 / ratio;
                    int gy = vy2 / ratio;
                    std::string bid = data.getTile(gx, gy);
                    if (bid.empty()) return 0;
                    auto it2 = group.blockIdToIntGrid.find(bid);
                    return it2 != group.blockIdToIntGrid.end() ? it2->second : 0;
                };

                int ruleIndex = 0;
                for (const auto& rule : group.rules) {
                    ++ruleIndex;
                    if (rule.chance < 1.0f) {
                        float rnd = (float)rand() / (float)RAND_MAX;
                        if (rnd > rule.chance) continue;
                    }

                    if (matchPattern(group, rule, vx, vy, getIntGridValue)) {
                        if (rule.uvs.empty()) continue;
                        int idx = rand() % (int)rule.uvs.size();
                        Rectangle uv = rule.uvs[idx];
                        
                        float px = (float)vx * group.tileSize + rule.xOffset;
                        float py = (float)vy * group.tileSize + rule.yOffset;
                        
                        if (vx >= 0 && vx < virtualWidth && vy >= 0 && vy < virtualHeight) {
                            int gx = vx / ratio;
                            int gy = vy / ratio;
                            std::string bid = data.getTile(gx, gy);
                            if (group.blockIdToIntGrid.count(bid)) {
                                autoTiledCells.insert(gy * data.width + gx);
                            }
                        }
                        
                        outputs.push_back({
                            px, py, uv, group.tilesetPath, group.tileSize, rule.f, group.groupLayerId, groupIndex, ruleIndex
                        });
                        
                        if (rule.breakOnMatch) {
                            layerHandled[group.groupLayerId] = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return outputs;
}
