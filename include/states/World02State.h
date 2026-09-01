#pragma once
#include "BaseLevelState.h"

// World 02 — map assets/maps/map02/world02.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World02State : public BaseLevelState {
public:
    World02State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map02/world02.ldtk", "", p1Name) {}
    explicit World02State(const GameSaveData& save) : BaseLevelState("assets/maps/map02/world02.ldtk", save) {}
    ~World02State() override = default;
};
