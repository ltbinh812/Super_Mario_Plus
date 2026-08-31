#pragma once
#include "BaseLevelState.h"

// World 04 — map assets/maps/map04/world04.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World04State : public BaseLevelState {
public:
    World04State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map04/world04.ldtk", "", p1Name) {}
    explicit World04State(const GameSaveData& save) : BaseLevelState("assets/maps/map04/world04.ldtk", save) {}
    ~World04State() override = default;
};
