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

    // Toàn bộ phần đọc nằm trong try: trước đây khối try chỉ bọc `file >> j`,
    // nên std::stoi trên khoá không phải số, .get<int>() trên uv thiếu phần tử,
    // hay operator[] const trên khoá vắng mặt đều thoát thẳng ra ngoài. Hàm này
    // chạy lúc VÀO EDITOR, nên một file rules hỏng là tắt game.
    try {
    for (const auto& rgJson : j) {
        if (!rgJson.is_object()) continue;

        AutoTileRuleGroup group;
        group.blockId         = rgJson.value("blockId", "");
        // ownIntGridValue có thể null nếu script trích xuất không tìm được mapping.
        // Dùng .value() thay cho operator[]: với nlohmann + NDEBUG, operator[]
        // const trên khoá vắng mặt là hành vi không xác định, không phải exception.
        group.ownIntGridValue = 1;
        if (rgJson.contains("ownIntGridValue") && rgJson["ownIntGridValue"].is_number()) {
            group.ownIntGridValue = rgJson["ownIntGridValue"].get<int>();
        }
        group.tilesetPath     = rgJson.value("tilesetPath", "");
        group.tilesetCols     = rgJson.value("tilesetCols", 1);
        group.tileSize        = rgJson.value("tileSize", 16);
        if (group.tileSize <= 0) group.tileSize = 16;   // tránh chia cho 0 phía sau

        group.groupLayerId = "default";
        if (rgJson.contains("groupLayerId") && rgJson["groupLayerId"].is_string()) {
            group.groupLayerId = rgJson["groupLayerId"].get<std::string>();
        }

        // Build intGridMapping — khoá là chuỗi số, phải phòng khoá rác.
        if (rgJson.contains("intGridMapping") && rgJson["intGridMapping"].is_object()) {
            for (auto it = rgJson["intGridMapping"].begin(); it != rgJson["intGridMapping"].end(); ++it) {
                if (!it.value().is_string()) continue;
                int intVal = 0;
                try { intVal = std::stoi(it.key()); }
                catch (const std::exception&) { continue; }   // khoá không phải số -> bỏ qua
                std::string bid = it.value().get<std::string>();
                group.intGridToBlockId[intVal] = bid;
                group.blockIdToIntGrid[bid] = intVal;
            }
        }

        // Đảm bảo blockId gốc của group cũng được map đúng với ownIntGridValue
        if (!group.blockId.empty()) {
            group.blockIdToIntGrid[group.blockId] = group.ownIntGridValue;
        }

        // Build groupMappings
        if (rgJson.contains("groupMappings") && rgJson["groupMappings"].is_object()) {
            for (auto it = rgJson["groupMappings"].begin(); it != rgJson["groupMappings"].end(); ++it) {
                if (!it.value().is_array()) continue;
                int groupId = 0;
                try { groupId = std::stoi(it.key()); }
                catch (const std::exception&) { continue; }
                std::vector<int> vals;
                for (const auto& v : it.value()) {
                    if (v.is_number()) vals.push_back(v.get<int>());
                }
                group.groupMappings[groupId] = vals;
            }
        }

        // Build rules
        for (const auto& rJson : rgJson.value("rules", json::array())) {
            if (!rJson.is_object()) continue;

            AutoTileRule rule;
            rule.size         = rJson.value("size", 3);
            rule.chance       = rJson.value("chance", 1.0f);
            rule.breakOnMatch = rJson.value("breakOnMatch", true);
            rule.xModulo      = rJson.value("xModulo", 1);
            rule.yModulo      = rJson.value("yModulo", 1);
            rule.f            = rJson.value("f", 0);
            if (rule.size <= 0)     rule.size = 3;
            if (rule.xModulo <= 0)  rule.xModulo = 1;
            if (rule.yModulo <= 0)  rule.yModulo = 1;

            if (rJson.contains("outOfBoundsValue") && rJson["outOfBoundsValue"].is_number()) {
                rule.outOfBoundsValue = rJson["outOfBoundsValue"].get<int>();
            } else {
                rule.outOfBoundsValue = std::nullopt;
            }

            for (const auto& pv : rJson.value("pattern", json::array())) {
                if (pv.is_number()) rule.pattern.push_back(pv.get<int>());
            }
            // Pattern phải đủ size*size ô, nếu không rule sẽ không bao giờ khớp
            // và im lặng — báo ra để còn biết file rules có vấn đề.
            if ((int)rule.pattern.size() != rule.size * rule.size) {
                std::cerr << "[AutoTiler] Rule cua '" << group.blockId
                          << "' co pattern " << rule.pattern.size()
                          << " o nhung size=" << rule.size << " (can "
                          << rule.size * rule.size << "). Bo qua rule nay.\n";
                continue;
            }

            // UVs — mỗi phần tử phải là mảng đủ 4 số.
            for (const auto& uvArr : rJson.value("uvs", json::array())) {
                if (!uvArr.is_array() || uvArr.size() < 4) continue;
                bool allNum = true;
                for (int k = 0; k < 4; ++k) if (!uvArr[k].is_number()) { allNum = false; break; }
                if (!allNum) continue;
                rule.uvs.push_back(Rectangle{
                    (float)uvArr[0].get<int>(), (float)uvArr[1].get<int>(),
                    (float)uvArr[2].get<int>(), (float)uvArr[3].get<int>() });
            }

            if (!rule.uvs.empty()) {
                group.rules.push_back(std::move(rule));
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
    } catch (const std::exception& e) {
        std::cerr << "[AutoTiler] Loi khi doc rules: " << e.what()
                  << " — giu lai " << groups_.size() << " group da doc duoc.\n";
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