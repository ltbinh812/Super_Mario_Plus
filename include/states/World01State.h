#pragma once
#include "BaseLevelState.h"

// World 01 — map assets/maps/map01/world01.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World01State : public BaseLevelState {
public:
    World01State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map01/world01.ldtk", "", p1Name) {}
    explicit World01State(const GameSaveData& save) : BaseLevelState("assets/maps/map01/world01.ldtk", save) {}
    ~World01State() override = default;
};
