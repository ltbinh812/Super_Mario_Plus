#pragma once
#include "BaseLevelState.h"

// World 03 — map assets/maps/map03/world03.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World03State : public BaseLevelState {
public:
    World03State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map03/world03.ldtk", "", p1Name) {}
    explicit World03State(const GameSaveData& save) : BaseLevelState("assets/maps/map03/world03.ldtk", save) {}
    ~World03State() override = default;
};
