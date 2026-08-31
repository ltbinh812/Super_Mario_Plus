#pragma once
#include "GameSaveData.h"
#include "SaveSlotInfo.h"
#include <vector>

// =============================================================================
// ISaveRepository — REPOSITORY PATTERN cho "kho chứa bản lưu".
//
// TRÁCH NHIỆM DUY NHẤT: trả lời 4 câu hỏi về BỘ SƯU TẬP bản lưu của một world:
//     - World này đang có những version nào?      listVersions()
//     - Tạo thêm một version mới đi.              createVersion()
//     - Nạp nội dung version này ra.              loadVersion()
//     - Xoá version này đi.                       deleteVersion()
//
// Nó KHÔNG biết dữ liệu được mã hoá kiểu gì (việc của ISaveSerializer), và
// gameplay KHÔNG biết bản lưu nằm ở thư mục nào (việc của Repository). Hai
// tầng này ghép lại qua constructor injection trong FileSaveRepository.
//
// Nhờ interface này, sau muốn đổi sang lưu trên cloud / SQLite chỉ cần viết
// CloudSaveRepository rồi SaveManager::setRepository() — gameplay không đổi.
// =============================================================================
class ISaveRepository {
public:
    virtual ~ISaveRepository() = default;

    // Liệt kê mọi bản lưu của world, ĐÃ SẮP XẾP mới nhất lên đầu.
    // World chưa từng lưu -> trả về vector rỗng (không phải lỗi).
    virtual std::vector<SaveSlotInfo> listVersions(int worldIndex) const = 0;

    // Tạo bản lưu MỚI (không ghi đè bản nào). `out` nhận thông tin file vừa tạo.
    virtual bool createVersion(int worldIndex, const GameSaveData& data, SaveSlotInfo& out) = 0;

    // Nạp trọn vẹn nội dung một bản lưu.
    virtual bool loadVersion(const SaveSlotInfo& slot, GameSaveData& out) const = 0;

    // Xoá vĩnh viễn một bản lưu khỏi đĩa.
    virtual bool deleteVersion(const SaveSlotInfo& slot) = 0;
};
