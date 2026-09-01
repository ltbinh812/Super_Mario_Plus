#include "EditorMapResizer.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>

// =============================================================================
// Helpers: thêm/xóa row/col trong sparse map
// =============================================================================

// Khi thêm row ở đầu (North): tất cả key cũ cần tăng gridY lên 1
void EditorMapResizer::expandNorth(CustomMapData& data) {
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        newTiles[(gy + 1) * data.width + gx] = id;
    }
    data.tiles = std::move(newTiles);
    for (auto& e : data.entities) e.gridY++;
    data.height++;
}

void EditorMapResizer::expandSouth(CustomMapData& data) {
    data.height++;  // sparse: không cần fill gì, ô mới tự động EMPTY
}

void EditorMapResizer::expandEast(CustomMapData& data) {
    // Cần remap key vì width thay đổi: key = gy * (newWidth) + gx
    int newW = data.width + 1;
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        newTiles[gy * newW + gx] = id;
    }
    data.tiles = std::move(newTiles);
    data.width = newW;
}

void EditorMapResizer::expandWest(CustomMapData& data) {
    int newW = data.width + 1;
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        newTiles[gy * newW + (gx + 1)] = id;  // shift gx sang phải 1
    }
    data.tiles = std::move(newTiles);
    for (auto& e : data.entities) e.gridX++;
    data.width = newW;
}

// Shrink: xóa row/col ngoài cùng, enforce min size
void EditorMapResizer::shrinkNorth(CustomMapData& data) {
    if (data.height <= MIN_SIZE) return;
    // Xóa tile ở gy=0, remap còn lại: gy → gy-1
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        if (gy == 0) continue;
        newTiles[(gy - 1) * data.width + gx] = id;
    }
    data.tiles = std::move(newTiles);
    data.entities.erase(std::remove_if(data.entities.begin(), data.entities.end(),
        [](const CustomEntityData& e){ return e.gridY == 0; }), data.entities.end());
    for (auto& e : data.entities) e.gridY--;
    data.height--;
}

void EditorMapResizer::shrinkSouth(CustomMapData& data) {
    if (data.height <= MIN_SIZE) return;
    int lastRow = data.height - 1;
    for (int gx = 0; gx < data.width; ++gx)
        data.tiles.erase(lastRow * data.width + gx);
    data.entities.erase(std::remove_if(data.entities.begin(), data.entities.end(),
        [&](const CustomEntityData& e){ return e.gridY == lastRow; }), data.entities.end());
    data.height--;
}

void EditorMapResizer::shrinkEast(CustomMapData& data) {
    if (data.width <= MIN_SIZE) return;
    int lastCol = data.width - 1;
    int newW    = data.width - 1;
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        if (gx == lastCol) continue;
        newTiles[gy * newW + gx] = id;
    }
    data.tiles = std::move(newTiles);
    data.entities.erase(std::remove_if(data.entities.begin(), data.entities.end(),
        [&](const CustomEntityData& e){ return e.gridX == lastCol; }), data.entities.end());
    data.width = newW;
}

void EditorMapResizer::shrinkWest(CustomMapData& data) {
    if (data.width <= MIN_SIZE) return;
    int newW = data.width - 1;
    std::unordered_map<int, std::string> newTiles;
    for (auto& [key, id] : data.tiles) {
        int gx = key % data.width;
        int gy = key / data.width;
        if (gx == 0) continue;
        newTiles[gy * newW + (gx - 1)] = id;
    }
    data.tiles = std::move(newTiles);
    data.entities.erase(std::remove_if(data.entities.begin(), data.entities.end(),
        [](const CustomEntityData& e){ return e.gridX == 0; }), data.entities.end());
    for (auto& e : data.entities) e.gridX--;
    data.width = newW;
}

// =============================================================================
// Handle rects (world space)
// =============================================================================

Rectangle EditorMapResizer::getHandleRect(Handle h, const CustomMapData& data, float wts) const {
    float mapW = data.width  * wts;
    float mapH = data.height * wts;
    float hs   = (float)HANDLE_SIZE;
    float cx   = mapW / 2.0f;
    float cy   = mapH / 2.0f;
    switch (h) {
        case Handle::North: return { cx - hs/2, -hs,   hs, hs };
        case Handle::South: return { cx - hs/2, mapH,  hs, hs };
        case Handle::West:  return { -hs,   cy - hs/2, hs, hs };
        case Handle::East:  return { mapW,  cy - hs/2, hs, hs };
        default:            return { 0,0,0,0 };
    }
}

// =============================================================================
// Render
// =============================================================================

void EditorMapResizer::render(const CustomMapData& data, float wts) const {
    auto drawHandle = [&](Handle h, Color c) {
        Rectangle r = getHandleRect(h, data, wts);
        DrawRectangleRec(r, c);
        DrawRectangleLinesEx(r, 2.0f, WHITE);
    };
    Color col = (dragging_ == Handle::None) ? Color{80, 200, 120, 220} : Color{255, 200, 50, 220};
    drawHandle(Handle::North, col);
    drawHandle(Handle::South, col);
    drawHandle(Handle::East,  col);
    drawHandle(Handle::West,  col);
}

// =============================================================================
// Process (drag logic)
// =============================================================================

bool EditorMapResizer::isHoveringHandle(const CustomMapData& data, float wts,
                                        Vector2 mouseWorld) const {
    for (auto h : { Handle::North, Handle::South, Handle::East, Handle::West }) {
        if (CheckCollisionPointRec(mouseWorld, getHandleRect(h, data, wts))) return true;
    }
    return false;
}

bool EditorMapResizer::process(CustomMapData& data, float wts, Vector2 mouseWorld) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Check nếu click vào handle
        for (auto h : { Handle::North, Handle::South, Handle::East, Handle::West }) {
            if (CheckCollisionPointRec(mouseWorld, getHandleRect(h, data, wts))) {
                dragging_     = h;
                dragStart_    = mouseWorld;
                dragStartDim_ = (h == Handle::East || h == Handle::West) ? data.width : data.height;
                return false;
            }
        }
    }

    bool changed = false;
    if (dragging_ != Handle::None && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float delta = 0;
        if (dragging_ == Handle::East)  delta = mouseWorld.x - dragStart_.x;
        if (dragging_ == Handle::West)  delta = dragStart_.x - mouseWorld.x;
        if (dragging_ == Handle::South) delta = mouseWorld.y - dragStart_.y;
        if (dragging_ == Handle::North) delta = dragStart_.y - mouseWorld.y;

        const bool horizontal = (dragging_ == Handle::East || dragging_ == Handle::West);

        // Kẹp kích thước đích vào [MIN_SIZE, MAX_SIZE] TRƯỚC vòng lặp. Nếu chỉ
        // dựa vào kiểm tra MIN_SIZE bên trong từng hàm shrink*, vòng while vẫn
        // quay đủ số lần mà không làm gì; còn phía nới rộng thì trước đây không
        // có trần nào cả — một cú kéo là treo máy.
        int newSize = dragStartDim_ + (int)(delta / wts);
        if (newSize < MIN_SIZE) newSize = MIN_SIZE;
        if (newSize > MAX_SIZE) newSize = MAX_SIZE;

        int curSize = horizontal ? data.width : data.height;
        int diff = newSize - curSize;

        while (diff > 0) {
            switch (dragging_) {
                case Handle::East:  expandEast(data);  break;
                case Handle::West:  expandWest(data);  break;
                case Handle::South: expandSouth(data); break;
                case Handle::North: expandNorth(data); break;
                default: break;
            }
            diff--; changed = true;
        }
        while (diff < 0) {
            switch (dragging_) {
                case Handle::East:  shrinkEast(data);  break;
                case Handle::West:  shrinkWest(data);  break;
                case Handle::South: shrinkSouth(data); break;
                case Handle::North: shrinkNorth(data); break;
                default: break;
            }
            diff++; changed = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        dragging_ = Handle::None;
    }

    return changed;
}

void EditorMapResizer::reset() {
    dragging_ = Handle::None;
}
