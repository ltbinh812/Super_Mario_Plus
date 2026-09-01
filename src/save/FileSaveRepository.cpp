#include "FileSaveRepository.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// -----------------------------------------------------------------------------
// Helper cục bộ: sinh chuỗi thời gian "YYYY-MM-DD HH:MM" để hiển thị trong panel.
// Tách riêng vì hàm đổi time_t sang struct tm an toàn có tên khác nhau tuỳ nền
// tảng: trên Windows (cả MSVC lẫn MinGW-w64) là localtime_s, trên POSIX là
// localtime_r. Gói sự khác biệt đó vào đúng một chỗ.
// -----------------------------------------------------------------------------
static std::string formatTimestamp(long long unixSeconds) {
    std::time_t t = static_cast<std::time_t>(unixSeconds);
    std::tm tmBuf{};
#if defined(_WIN32)
    localtime_s(&tmBuf, &t);
#else
    localtime_r(&t, &tmBuf);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tmBuf);
    return std::string(buf);
}

// -----------------------------------------------------------------------------

FileSaveRepository::FileSaveRepository(std::unique_ptr<ISaveSerializer> serializer,
                                       const std::string& rootDir)
    : serializer_(std::move(serializer)), rootDir_(rootDir) {}

std::string FileSaveRepository::worldDir(int worldIndex) const {
    // world 1..9 -> "world01".."world09";  world >= 10 -> "world10"
    std::string num = std::to_string(worldIndex);
    if (worldIndex < 10) num = "0" + num;
    return rootDir_ + "/world" + num;
}

std::string FileSaveRepository::versionPath(int worldIndex, int versionIndex) const {
    return worldDir(worldIndex) + "/version" + std::to_string(versionIndex) + ".json";
}

bool FileSaveRepository::ensureWorldDir(int worldIndex) const {
    std::error_code ec;
    fs::create_directories(worldDir(worldIndex), ec);
    if (ec) {
        std::cerr << "[FileSaveRepository] Khong tao duoc thu muc " << worldDir(worldIndex)
                  << ": " << ec.message() << "\n";
        return false;
    }
    return true;
}

int FileSaveRepository::parseVersionIndex(const std::string& fileName) {
    // Chấp nhận đúng mẫu "version<N>.json" với N là chuỗi chữ số không rỗng.
    const std::string prefix = "version";
    const std::string suffix = ".json";
    if (fileName.size() <= prefix.size() + suffix.size()) return -1;
    if (fileName.compare(0, prefix.size(), prefix) != 0) return -1;
    if (fileName.compare(fileName.size() - suffix.size(), suffix.size(), suffix) != 0) return -1;

    std::string digits = fileName.substr(prefix.size(),
                                         fileName.size() - prefix.size() - suffix.size());
    if (digits.empty()) return -1;
    for (char c : digits) {
        if (c < '0' || c > '9') return -1;
    }

    try {
        return std::stoi(digits);
    } catch (const std::exception&) {
        return -1;  // số quá lớn -> coi như không hợp lệ
    }
}

std::vector<SaveSlotInfo> FileSaveRepository::listVersions(int worldIndex) const {
    std::vector<SaveSlotInfo> result;
    if (worldIndex < 0 || !serializer_) return result;

    const std::string dir = worldDir(worldIndex);
    std::error_code ec;
    if (!fs::exists(dir, ec)) return result;  // world chưa từng lưu -> danh sách rỗng

    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        if (!entry.is_regular_file()) continue;

        const std::string fileName = entry.path().filename().string();
        int idx = parseVersionIndex(fileName);
        if (idx < 0) continue;

        SaveSlotInfo info;
        info.filePath     = dir + "/" + fileName;
        info.versionIndex = idx;
        // File hỏng thì bỏ qua hẳn, KHÔNG đưa vào danh sách và KHÔNG crash —
        // người chơi vẫn load được các bản lưu lành lặn còn lại.
        if (!serializer_->readMeta(info.filePath, info.meta)) {
            std::cerr << "[FileSaveRepository] Bo qua ban luu hong: " << info.filePath << "\n";
            continue;
        }
        info.meta.versionIndex = idx;  // tên file là nguồn chân lý cho số version
        info.isValid = true;
        result.push_back(std::move(info));
    }

    // Mới nhất lên đầu: ưu tiên versionIndex (luôn tăng dần theo thời gian lưu).
    std::sort(result.begin(), result.end(),
              [](const SaveSlotInfo& a, const SaveSlotInfo& b) {
                  return a.versionIndex > b.versionIndex;
              });
    return result;
}

bool FileSaveRepository::createVersion(int worldIndex, const GameSaveData& data, SaveSlotInfo& out) {
    if (worldIndex < 0 || !serializer_) return false;
    if (!ensureWorldDir(worldIndex)) return false;

    // Số version kế tiếp = max hiện có + 1 (bắt đầu từ 1).
    int nextIndex = 1;
    for (const auto& slot : listVersions(worldIndex)) {
        if (slot.versionIndex >= nextIndex) nextIndex = slot.versionIndex + 1;
    }

    // Bổ sung nốt phần metadata mà chỉ Repository mới biết: số thứ tự và thời điểm.
    GameSaveData stamped = data;
    stamped.meta.schemaVersion = 2;
    stamped.meta.versionIndex  = nextIndex;
    stamped.meta.worldIndex    = worldIndex;
    stamped.meta.savedAtUnix   = static_cast<long long>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    stamped.meta.savedAtText   = formatTimestamp(stamped.meta.savedAtUnix);

    const std::string path = versionPath(worldIndex, nextIndex);
    if (!serializer_->write(stamped, path)) return false;

    out.filePath     = path;
    out.versionIndex = nextIndex;
    out.meta         = stamped.meta;
    out.isValid      = true;
    return true;
}

bool FileSaveRepository::loadVersion(const SaveSlotInfo& slot, GameSaveData& out) const {
    if (!serializer_ || slot.filePath.empty()) return false;
    return serializer_->read(slot.filePath, out);
}

bool FileSaveRepository::deleteVersion(const SaveSlotInfo& slot) {
    if (slot.filePath.empty()) return false;

    std::error_code ec;
    bool removed = fs::remove(slot.filePath, ec);
    if (ec || !removed) {
        std::cerr << "[FileSaveRepository] Khong xoa duoc " << slot.filePath
                  << (ec ? (": " + ec.message()) : std::string(": file khong ton tai")) << "\n";
        return false;
    }
    std::cout << "[FileSaveRepository] Da xoa ban luu: " << slot.filePath << "\n";
    return true;
}
