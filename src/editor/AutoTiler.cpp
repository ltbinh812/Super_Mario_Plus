#include "AutoTiler.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// =============================================================================
// loadRules — Parse format mới (list of RuleGroup)
// =============================================================================
void AutoTiler::loadRules(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[AutoTiler] Failed to open: " << filepath << "\n";
        return;
    }

    json j;
    try {
        file >> j;
    } catch (std::exception& e) {
        std::cerr << "[AutoTiler] JSON parse error: " << e.what() << "\n";
        return;
    }

    groups_.clear();
    blockToGroupIndex_.clear();

    if (!j.is_array()) {
        std::cerr << "[AutoTiler] Expected JSON array of rule groups\n";
        return;
    }

    for (const auto& rgJson : j) {
        AutoTileRuleGroup group;
        group.blockId         = rgJson.value("blockId", "");
        // ownIntGridValue có thể là null nếu Python script không tìm được mapping → default về 1
        auto ownVal = rgJson["ownIntGridValue"];
        group.ownIntGridValue = (ownVal.is_number()) ? ownVal.get<int>() : 1;
        group.tilesetPath     = rgJson.value("tilesetPath", "");
        group.tilesetCols     = rgJson.value("tilesetCols", 1);
        group.tileSize        = rgJson.value("tileSize", 16);

        if (rgJson.contains("groupLayerId") && rgJson["groupLayerId"].is_string()) {
            group.groupLayerId = rgJson["groupLayerId"].get<std::string>();
        } else {
            group.groupLayerId = "default";
        }

        // Build intGridMapping
        if (rgJson.contains("intGridMapping") && rgJson["intGridMapping"].is_object()) {
            for (auto it = rgJson["intGridMapping"].begin(); it != rgJson["intGridMapping"].end(); ++it) {
                int intVal = std::stoi(it.key());
                std::string bid = it.value().get<std::string>();
                group.intGridToBlockId[intVal] = bid;
                group.blockIdToIntGrid[bid] = intVal;
            }
        }
        
        // Đảm bảo blockId gốc của group cũng được map đúng với ownIntGridValue
        // (Để khi user đặt khối WORLD03_THIN_PLATFORMS, nó sẽ trigger các rules của group thin platforms)
        if (!group.blockId.empty()) {
            group.blockIdToIntGrid[group.blockId] = group.ownIntGridValue;
        }

        // Build groupMappings
        if (rgJson.contains("groupMappings") && rgJson["groupMappings"].is_object()) {
            for (auto it = rgJson["groupMappings"].begin(); it != rgJson["groupMappings"].end(); ++it) {
                int groupId = std::stoi(it.key());
                std::vector<int> vals;
                for (const auto& v : it.value()) {
                    vals.push_back(v.get<int>());
                }
                group.groupMappings[groupId] = vals;
            }
        }

        // Build rules
        for (const auto& rJson : rgJson.value("rules", json::array())) {
            AutoTileRule rule;
            rule.size    = rJson.value("size", 3);
            rule.chance       = rJson.value("chance", 1.0f);
            rule.breakOnMatch = rJson.value("breakOnMatch", true);
            rule.xModulo      = rJson.value("xModulo", 1);
            rule.yModulo      = rJson.value("yModulo", 1);
            rule.f            = rJson.value("f", 0);
            
            if (rJson.contains("outOfBoundsValue") && !rJson["outOfBoundsValue"].is_null()) {
                rule.outOfBoundsValue = rJson["outOfBoundsValue"].get<int>();
            } else {
                rule.outOfBoundsValue = std::nullopt;
            }

            // Pattern: raw integers giống LDtk gốc
            for (const auto& pv : rJson.value("pattern", json::array())) {
                rule.pattern.push_back(pv.get<int>());
            }

            // UVs
            for (const auto& uvArr : rJson.value("uvs", json::array())) {
                Rectangle rect = {
                    (float)uvArr[0].get<int>(),
                    (float)uvArr[1].get<int>(),
                    (float)uvArr[2].get<int>(),
                    (float)uvArr[3].get<int>()
                };
                rule.uvs.push_back(rect);
            }

            if (!rule.uvs.empty()) {
                group.rules.push_back(rule);
            }
        }

        if (!group.blockId.empty() && !group.rules.empty()) {
            int idx = (int)groups_.size();
            // Register tất cả blockId trong group (không chỉ chính nó)
            for (const auto& [val, bid] : group.intGridToBlockId) {
                // Chỉ override nếu chưa có (tránh ghi đè)
                if (blockToGroupIndex_.find(bid) == blockToGroupIndex_.end()) {
                    blockToGroupIndex_[bid] = idx;
                }
            }
            // Chính group này luôn được đăng ký
            blockToGroupIndex_[group.blockId] = idx;
            groups_.push_back(std::move(group));
        }
    }

    std::cout << "[AutoTiler] Loaded " << groups_.size() << " rule groups ("
              << blockToGroupIndex_.size() << " block mappings)\n";
}

// =============================================================================
// matchPattern — So sánh pattern với IntGrid values thực tế tại (cx, cy)
// =============================================================================
bool AutoTiler::matchPattern(const AutoTileRuleGroup& group, const AutoTileRule& rule, int cx, int cy,
                              const std::function<int(int, int, std::optional<int>)>& getIntGridValue) const {
    if (rule.xModulo > 1 && (cx % rule.xModulo) != 0) return false;
    if (rule.yModulo > 1 && (cy % rule.yModulo) != 0) return false;

    int radius = rule.size / 2;
    int idx = 0;

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (idx >= (int)rule.pattern.size()) return false;
            int patVal = rule.pattern[idx++];

            // 0 = Any (don't care)
            if (patVal == 0) continue;

            int actualVal = getIntGridValue(cx + dx, cy + dy, rule.outOfBoundsValue);

            bool inverse  = (patVal < 0);
            int  absVal   = std::abs(patVal);

            bool match;
            if (absVal == 1000001) {
                // Any non-empty
                match = (actualVal != 0);
            } else if (absVal >= 2000) {
                // Group matching (e.g. 2000 + groupUid)
                int groupId = absVal - 2000;
                auto it = group.groupMappings.find(groupId);
                if (it != group.groupMappings.end()) {
                    match = false;
                    for (int gval : it->second) {
                        if (actualVal == gval) {
                            match = true;
                            break;
                        }
                    }
                } else {
                    match = false;
                }
            } else {
                match = (actualVal == absVal);
            }

            // inverse XOR match → nếu điều kiện fail thì return false
            if (inverse ? match : !match) return false;
        }
    }

    return true;
}