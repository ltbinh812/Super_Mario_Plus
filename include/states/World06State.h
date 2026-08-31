#pragma once
#include "BaseLevelState.h"

// World 06 — map assets/maps/map06/world06.ldtk
// Lớp vỏ mỏng của BaseLevelState, chỉ cố định đường dẫn map. Hai constructor
// tương ứng hai lối vào màn chơi, cả hai đều do WorldCatalog gọi:
//   - (p1Name) : NEW GAME  — bắt đầu ở level chứa Starting_position
//   - (save)   : LOAD GAME — khôi phục level/nhân vật/trạng thái từ bản lưu
class World06State : public BaseLevelState {
public:
    World06State(const std::string& p1Name = "Goku") : BaseLevelState("assets/maps/map06/world06.ldtk", "", p1Name) {}
    explicit World06State(const GameSaveData& save) : BaseLevelState("assets/maps/map06/world06.ldtk", save) {}
    ~World06State() override = default;
};
