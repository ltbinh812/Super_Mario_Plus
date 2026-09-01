#pragma once
#include "SaveData.h"
#include "ISaveRepository.h"
#include <memory>
#include <string>
#include <vector>

// =============================================================================
// SaveManager — FACADE + SINGLETON: cửa duy nhất cho gameplay nói chuyện với
// hệ thống lưu trữ.
//
// Nó gánh HAI vai trò khác nhau, đừng nhầm lẫn:
//
//  1. CHECKPOINT TRONG RAM (phần cũ, giữ nguyên hành vi)
//     setCheckpoint/getCheckpoint/hasCheckpoint/clearCheckpoint.
//     Dùng cho việc hồi sinh khi chết — không đụng tới đĩa, rất nhanh.
//
//  2. BẢN LƯU TRÊN ĐĨA (phần mới)
//     listVersions/createVersion/loadVersion/deleteVersion.
//     Uỷ quyền toàn bộ cho ISaveRepository; SaveManager không hề biết
//     std::filesystem hay JSON là gì.
//
// Repository được khởi tạo lười (lazy): lần đầu ai đó cần tới thì mới dựng
// FileSaveRepository + JsonSaveSerializer. Test có thể gọi setRepository()
// trước để tiêm bản giả.
// =============================================================================
class SaveManager {
private:
    GameSaveData currentCheckpoint;
    bool hasCheckpointData = false;

    // mutable vì listVersions()/loadVersion() là const về mặt ngữ nghĩa nhưng
    // vẫn cần khởi tạo lười repository ở lần gọi đầu tiên.
    mutable std::unique_ptr<ISaveRepository> repository_;

    SaveManager() = default;

    // Trả về repository, dựng mặc định nếu chưa có.
    ISaveRepository& repo() const;

public:
    static SaveManager& getInstance() {
        static SaveManager instance;
        return instance;
    }

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // ---- 1. Checkpoint trong RAM (API cũ, không đổi) ----------------------
    void setCheckpoint(const GameSaveData& data) {
        currentCheckpoint = data;
        hasCheckpointData = true;
    }

    GameSaveData getCheckpoint() const {
        return currentCheckpoint;
    }

    bool hasCheckpoint() const {
        return hasCheckpointData;
    }

    void clearCheckpoint() {
        hasCheckpointData = false;
        currentCheckpoint = GameSaveData{};
    }

    // Ghi/đọc thẳng một đường dẫn cụ thể. Giữ lại để tương thích ngược; nay chỉ
    // là lớp vỏ mỏng gọi serializer thay vì tự dựng JSON bằng tay.
    bool saveToFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath);

    // ---- 2. Bản lưu nhiều version trên đĩa (API mới) ----------------------

    // Tiêm repository khác (Dependency Injection). Truyền nullptr để quay lại mặc định.
    void setRepository(std::unique_ptr<ISaveRepository> repository);

    // Danh sách bản lưu của một world, mới nhất lên đầu.
    std::vector<SaveSlotInfo> listVersions(int worldIndex) const;

    // Tạo bản lưu mới trong saves/world0X/. `out` nhận thông tin file vừa tạo.
    bool createVersion(int worldIndex, const GameSaveData& data, SaveSlotInfo& out);

    // Nạp nội dung một bản lưu đã chọn trong panel.
    bool loadVersion(const SaveSlotInfo& slot, GameSaveData& out) const;

    // Xoá vĩnh viễn một bản lưu (nút DELETE).
    bool deleteVersion(const SaveSlotInfo& slot);
};
