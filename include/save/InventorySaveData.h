#pragma once

// =============================================================================
// InventorySaveData — Ảnh chụp túi đồ DÙNG CHUNG của cả party.
//
// Tương ứng 1-1 với struct PartyInventory (include/entity/Player/CharacterStats.h),
// vốn được chia sẻ giữa P1 và P2 qua std::shared_ptr<PartyInventory>.
//
// AI TẠO RA NÓ?  BaseLevelState::createSaveData()
// AI ĐỌC NÓ?     BaseLevelState::restoreFromSaveData(), JsonSaveSerializer
// =============================================================================
struct InventorySaveData {
    int coins = 0;
    int keys  = 0;
};
