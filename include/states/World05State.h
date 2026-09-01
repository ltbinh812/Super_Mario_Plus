#pragma once
#include "BaseLevelState.h"

// World 05 — map assets/maps/map05/world05.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World05State : public BaseLevelState {
public:
    World05State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map05/world05.ldtk", "", p1Name) {}
    explicit World05State(const GameSaveData& save) : BaseLevelState("assets/maps/map05/world05.ldtk", save) {}
    ~World05State() override = default;
};
