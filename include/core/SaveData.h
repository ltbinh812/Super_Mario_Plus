#pragma once

// =============================================================================
// SaveData.h — AGGREGATOR HEADER (giữ để tương thích ngược).
//
// Trước đây file này chứa thẳng 4 struct PlayerSaveData / InventorySaveData /
// LevelSaveData / GameSaveData. Theo quy tắc "1 Class / 1 File" của dự án
// (.agents/rules/oop-enforcer), mỗi struct đã được tách ra file riêng trong
// include/save/.
//
// File này KHÔNG bị xoá: mọi chỗ đang `#include "SaveData.h"` (BaseLevelState.h,
// SaveManager.h, ...) vẫn biên dịch bình thường, không phải sửa một dòng nào.
//
// Code mới nên include trực tiếp header cụ thể mình cần, ví dụ:
//     #include "GameSaveData.h"      thay vì  #include "SaveData.h"
// =============================================================================

#include "PlayerSaveData.h"
#include "InventorySaveData.h"
#include "LevelSaveData.h"
#include "SaveMetaData.h"
#include "GameSaveData.h"
#include "SaveSlotInfo.h"
