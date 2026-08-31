#pragma once
#include "ISaveSerializer.h"

// =============================================================================
// JsonSaveSerializer — Cài đặt cụ thể của ISaveSerializer bằng nlohmann/json.
//
// Toàn bộ ánh xạ struct <-> JSON nằm gọn trong JsonSaveSerializer.cpp, KHÔNG
// lộ ra header này. Nhờ vậy BaseLevelState.h (và mọi file include nó) không
// phải nuốt 25.000 dòng của nlohmann/json.hpp mỗi lần biên dịch.
//
// Class không có state -> mọi method đều const, dùng chung một instance được.
// =============================================================================
class JsonSaveSerializer : public ISaveSerializer {
public:
    JsonSaveSerializer() = default;
    ~JsonSaveSerializer() override = default;

    bool write(const GameSaveData& data, const std::string& filePath) const override;
    bool read(const std::string& filePath, GameSaveData& out) const override;
    bool readMeta(const std::string& filePath, SaveMetaData& out) const override;
};
