#include "WorldCatalog.h"
#include "World01State.h"
#include "World02State.h"
#include "World03State.h"
#include "World04State.h"
#include "World05State.h"
#include "World06State.h"
#include <iostream>

WorldCatalog& WorldCatalog::getInstance() {
    static WorldCatalog instance;
    return instance;
}

WorldCatalog::WorldCatalog() {
    registerWorlds();
}

// -----------------------------------------------------------------------------
// ĐĂNG KÝ 6 WORLD — nơi duy nhất trong toàn dự án ánh xạ số hiệu -> class state.
//
// Muốn thêm World07: viết World07State.h rồi chép thêm một khối dưới đây. Không
// phải sửa MapSelectionState, không phải sửa hệ save.
//
// Lưu ý về lambda: makeNew/makeLoaded chỉ bắt giá trị theo tham số, không bắt
// `this` — nên chúng an toàn kể cả khi được copy ra ngoài rồi gọi sau (ví dụ
// LoadingState giữ factory qua vài frame mới thực thi).
// -----------------------------------------------------------------------------
void WorldCatalog::registerWorlds() {
    // Macro cục bộ chỉ để giảm lặp trong đúng hàm này; #undef ngay sau khi xong.
    #define REGISTER_WORLD(N, CLASS, PATH)                                       \
        {                                                                        \
            WorldDescriptor d;                                                   \
            d.index       = N;                                                   \
            d.displayName = "WORLD 0" #N;                                        \
            d.mapFilePath = PATH;                                                \
            d.makeNew     = [](const std::string& p1Name)                        \
                            -> std::unique_ptr<GameState> {                      \
                                return std::make_unique<CLASS>(p1Name);          \
                            };                                                   \
            d.makeLoaded  = [](const GameSaveData& save)                         \
                            -> std::unique_ptr<GameState> {                      \
                                return std::make_unique<CLASS>(save);            \
                            };                                                   \
            worlds_.push_back(std::move(d));                                     \
        }

    REGISTER_WORLD(1, World01State, "assets/maps/map01/world01.ldtk")
    REGISTER_WORLD(2, World02State, "assets/maps/map02/world02.ldtk")
    REGISTER_WORLD(3, World03State, "assets/maps/map03/world03.ldtk")
    REGISTER_WORLD(4, World04State, "assets/maps/map04/world04.ldtk")
    REGISTER_WORLD(5, World05State, "assets/maps/map05/world05.ldtk")
    REGISTER_WORLD(6, World06State, "assets/maps/map06/world06.ldtk")

    #undef REGISTER_WORLD
}

const WorldDescriptor* WorldCatalog::find(int index) const {
    for (const auto& w : worlds_) {
        if (w.index == index) return &w;
    }
    return nullptr;
}

int WorldCatalog::indexFromMapPath(const std::string& mapFilePath) const {
    // So khớp CHÍNH XÁC đường dẫn đã đăng ký. Cố ý không dùng cách "tìm chuỗi
    // map0N" cho dễ dãi: các map PvP nằm ở assets/maps/pvp_map03/world03.ldtk
    // cũng chứa "world03.ldtk", nếu khớp lỏng sẽ vô tình auto-save cả ván PvP.
    for (const auto& w : worlds_) {
        if (w.mapFilePath == mapFilePath) return w.index;
    }
    return -1;
}

std::string WorldCatalog::displayName(int index) const {
    const WorldDescriptor* d = find(index);
    return d ? d->displayName : std::string("");
}

std::unique_ptr<GameState> WorldCatalog::createNew(int index, const std::string& p1Name) const {
    const WorldDescriptor* d = find(index);
    if (!d || !d->makeNew) {
        std::cerr << "[WorldCatalog] Khong co world index " << index << " de tao moi.\n";
        return nullptr;
    }
    return d->makeNew(p1Name);
}

std::unique_ptr<GameState> WorldCatalog::createLoaded(int index, const GameSaveData& save) const {
    const WorldDescriptor* d = find(index);
    if (!d || !d->makeLoaded) {
        std::cerr << "[WorldCatalog] Khong co world index " << index << " de nap ban luu.\n";
        return nullptr;
    }
    return d->makeLoaded(save);
}
