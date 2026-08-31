#pragma once
#include "SaveMetaData.h"
#include <string>

// =============================================================================
// SaveSlotInfo — Mô tả MỘT FILE SAVE NẰM TRÊN ĐĨA (chưa nạp nội dung).
//
// PHÂN BIỆT VỚI GameSaveData:
//   SaveSlotInfo  = "có file này, ở đường dẫn này, bìa sách nói thế này"
//   GameSaveData  = "đây là toàn bộ nội dung đã nạp vào RAM"
//
// SaveVersionPanel làm việc hoàn toàn với SaveSlotInfo (nhẹ, chỉ đọc meta).
// Chỉ khi người chơi bấm nút LOAD ta mới gọi SaveManager::loadVersion() để
// nạp GameSaveData thật. Nhờ vậy mở panel có 20 bản lưu vẫn không giật.
//
// AI TẠO RA NÓ?  FileSaveRepository::listVersions() / createVersion()
// AI ĐỌC NÓ?     SaveVersionPanel (vẽ), MapSelectionState (chuyển cho load/delete)
// =============================================================================
struct SaveSlotInfo {
    std::string  filePath = "";   // "saves/world03/version3.json"
    int          versionIndex = 0;
    SaveMetaData meta;
    bool         isValid = false; // false = file hỏng / không đọc được meta
};
