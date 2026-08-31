#pragma once
#include "ItemState.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

// =============================================================================
// LevelSaveData — Ảnh chụp TRẠNG THÁI MAP đã bị người chơi làm thay đổi.
//
// Ý tưởng cốt lõi: KHÔNG lưu lại toàn bộ map (map gốc đã nằm sẵn trong file
// .ldtk). Ta chỉ lưu phần "sai khác so với trạng thái ban đầu", định danh bằng
// IID — chuỗi UUID mà LDtk gán cho từng entity và không bao giờ đổi.
//
//   persistedDeadEntities : tập IID của quái/boss đã bị tiêu diệt.
//                           spawnEntitiesFromMap() thấy IID nằm trong tập này
//                           thì bỏ qua, không spawn lại  -> quái ở luôn dưới mồ.
//   persistedItemStates   : IID -> ItemState. Rương đã mở, cờ đã cắm, cửa đã
//                           mở khoá... đều được khôi phục đúng trạng thái.
//
// Vì IID là duy nhất trên TOÀN BỘ file .ldtk (chứ không chỉ trong 1 level), hai
// container này bao trọn mọi level của world mà không cần lồng thêm tầng nào.
//
// AI TẠO RA NÓ?  BaseLevelState::createSaveData()
// AI ĐỌC NÓ?     BaseLevelState::restoreFromSaveData() -> spawnEntitiesFromMap()
// =============================================================================
struct LevelSaveData {
    std::string worldId;      // "world03"
    int         worldIndex = -1;  // 3  (-1 = không thuộc world 1-player nào)
    std::string levelId;      // Level LDtk đang đứng, vd "World_Level_4"
    std::string mapFilePath;  // "assets/maps/map03/world03.ldtk"

    std::unordered_map<std::string, ItemState> persistedItemStates;
    std::unordered_set<std::string>            persistedDeadEntities;
};
