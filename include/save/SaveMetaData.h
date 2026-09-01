#pragma once
#include <string>

// =============================================================================
// SaveMetaData — Phần "bìa sách" của một file save.
//
// VÌ SAO PHẢI TÁCH RIÊNG?
// Panel LOAD GAME cần liệt kê hàng chục bản lưu cùng lúc, mỗi dòng hiện: nhân
// vật nào, đang ở level nào, bao nhiêu coin, máu còn bao nhiêu, lưu lúc mấy giờ.
// Nếu không có khối meta này thì mỗi lần mở panel phải parse trọn vẹn từng file
// (bao gồm hàng trăm IID trong persistedDeadEntities) chỉ để lấy vài con số.
// Có meta -> ISaveSerializer::readMeta() chỉ đọc đúng object nhỏ ở đầu file.
//
// Dữ liệu ở đây là BẢN SAO của một số trường trong thân save. Cố ý dư thừa để
// đổi lấy tốc độ hiển thị danh sách.
//
// AI TẠO RA NÓ?  BaseLevelState::createSaveData() (điền phần lớn) +
//                FileSaveRepository::createVersion() (điền versionIndex, thời gian)
// AI ĐỌC NÓ?     SaveVersionPanel — để vẽ từng dòng trong danh sách
// =============================================================================
struct SaveMetaData {
    // Tăng con số này mỗi khi đổi cấu trúc file save, để sau còn biết đường
    // migrate. Schema 1 = định dạng save.json cũ (1 file phẳng, không có meta).
    int schemaVersion = 2;

    int versionIndex = 0;        // N trong "versionN.json"
    long long savedAtUnix = 0;   // epoch seconds — dùng để sắp xếp
    std::string savedAtText = ""; // "2026-08-31 21:40" — dùng để hiển thị

    int worldIndex = -1;          // 1..6
    std::string levelId = "";     // "World_Level_4"
    std::string characterName = ""; // "Goku"

    float playTimeSeconds = 0.0f;

    int coins     = 0;
    int health    = 0;
    int maxHealth = 0;
};
