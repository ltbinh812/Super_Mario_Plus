#pragma once
#include "GameSaveData.h"
#include "SaveMetaData.h"
#include <string>

// =============================================================================
// ISaveSerializer — STRATEGY PATTERN cho "định dạng lưu trữ".
//
// TRÁCH NHIỆM DUY NHẤT: biến GameSaveData <-> chuỗi byte trong một file.
// Nó KHÔNG biết gì về thư mục, đánh số version hay world nào — việc đó là của
// ISaveRepository. Tách đôi như vậy để:
//
//   1. Muốn đổi sang định dạng nhị phân (nhanh hơn, chống sửa tay) chỉ cần viết
//      BinarySaveSerializer, không đụng một dòng nào ở Repository hay gameplay.
//   2. Test được: có thể cắm một serializer giả (in-memory) khi viết unit test.
//
// readMeta() tồn tại riêng vì panel LOAD GAME chỉ cần "bìa sách" của hàng chục
// file — đọc đủ meta rẻ hơn rất nhiều so với parse trọn vẹn từng save.
// =============================================================================
class ISaveSerializer {
public:
    virtual ~ISaveSerializer() = default;

    // Ghi toàn bộ save xuống filePath. Trả false nếu không mở/ghi được file.
    virtual bool write(const GameSaveData& data, const std::string& filePath) const = 0;

    // Nạp toàn bộ save từ filePath vào `out`. Trả false nếu file thiếu/hỏng.
    virtual bool read(const std::string& filePath, GameSaveData& out) const = 0;

    // Chỉ nạp phần meta — dùng khi liệt kê danh sách bản lưu.
    virtual bool readMeta(const std::string& filePath, SaveMetaData& out) const = 0;
};
