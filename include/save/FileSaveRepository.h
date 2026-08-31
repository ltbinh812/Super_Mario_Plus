#pragma once
#include "ISaveRepository.h"
#include "ISaveSerializer.h"
#include <memory>
#include <string>

// =============================================================================
// FileSaveRepository — Cài đặt ISaveRepository trên hệ thống file cục bộ.
//
// BỐ CỤC THƯ MỤC:
//     saves/
//       world01/  version1.json  version2.json  ...
//       world03/  version1.json  ...
//       world06/  ...
//
// Mỗi world một thư mục riêng -> chơi world 3 rồi nhảy sang world 5 không hề
// đụng chạm bản lưu của nhau (bug cũ: mọi world ghi chung saves/save.json).
//
// Số version tăng dần, KHÔNG tái sử dụng: xoá version2 rồi lưu tiếp vẫn ra
// version4 nếu version3 đang tồn tại. Như vậy tên file luôn ổn định, người
// chơi không bị "bản lưu tự đổi số".
//
// DEPENDENCY INJECTION: serializer được truyền vào constructor thay vì new
// cứng bên trong -> đổi định dạng lưu không phải sửa class này.
// =============================================================================
class FileSaveRepository : public ISaveRepository {
private:
    std::unique_ptr<ISaveSerializer> serializer_;
    std::string rootDir_;

    // "saves/world03"  — tự đệm số 0 cho world 1..9
    std::string worldDir(int worldIndex) const;
    // "saves/world03/version3.json"
    std::string versionPath(int worldIndex, int versionIndex) const;
    // Bảo đảm thư mục world tồn tại; trả false nếu tạo thất bại.
    bool ensureWorldDir(int worldIndex) const;
    // Rút số N từ tên file "versionN.json"; trả -1 nếu không khớp mẫu.
    static int parseVersionIndex(const std::string& fileName);

public:
    // rootDir mặc định "saves" — tham số hoá để test có thể trỏ vào thư mục tạm.
    explicit FileSaveRepository(std::unique_ptr<ISaveSerializer> serializer,
                                const std::string& rootDir = "saves");
    ~FileSaveRepository() override = default;

    std::vector<SaveSlotInfo> listVersions(int worldIndex) const override;
    bool createVersion(int worldIndex, const GameSaveData& data, SaveSlotInfo& out) override;
    bool loadVersion(const SaveSlotInfo& slot, GameSaveData& out) const override;
    bool deleteVersion(const SaveSlotInfo& slot) override;
};
