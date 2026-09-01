#pragma once
#include "SaveMetaData.h"
#include "PlayerSaveData.h"
#include "InventorySaveData.h"
#include "LevelSaveData.h"

// =============================================================================
// GameSaveData — Aggregate Root của toàn bộ hệ thống save.
//
// Gom 4 mảnh dữ liệu độc lập thành một đơn vị duy nhất đi qua đĩa:
//   meta      : bìa sách, để panel liệt kê nhanh          (SaveMetaData)
//   p1        : trạng thái người chơi 1                    (PlayerSaveData)
//   inventory : coin + key dùng chung                      (InventorySaveData)
//   levelData : sai khác của map so với file .ldtk gốc     (LevelSaveData)
//
// Chỉ có p1 vì tính năng save hiện chỉ phục vụ chế độ 1-Player. Muốn mở rộng
// sang 2-Player về sau chỉ cần thêm "PlayerSaveData p2;" — mọi tầng khác
// (Serializer, Repository, Panel) không phải sửa cấu trúc.
//
// isValid = false nghĩa là "chưa có dữ liệu"; cả saveToFile lẫn
// restoreFromSaveData đều kiểm tra cờ này trước khi làm gì.
// =============================================================================
struct GameSaveData {
    bool isValid = false;

    SaveMetaData      meta;
    PlayerSaveData    p1;
    InventorySaveData inventory;
    LevelSaveData     levelData;
};
