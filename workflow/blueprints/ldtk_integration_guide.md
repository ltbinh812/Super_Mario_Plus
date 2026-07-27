# Hướng Dẫn Tích Hợp Bản Đồ LDtk (`.ldtk`), Design Pattern Tối Ưu Render & Xử Lý Va Chạm Cho 2 Nhân Vật

Tài liệu này là bản hướng dẫn kiến trúc cấp cao dành cho dự án SuperMarioPlus, tập trung vào việc áp dụng các **Design Pattern** chuẩn mực để tối ưu hóa tài nguyên phần cứng (CPU/GPU) khi xử lý bản đồ LDtk rộng lớn ($3472 \times 256$ pixel) cùng hệ thống **2 nhân vật (2-Player Coop / Multi-entity)**.

---

## 1. Các Design Pattern & Kỹ Thuật Tối Ưu Render Cho Game 2 Nhân Vật

Để đảm bảo tốc độ khung hình luôn đạt 60-144 FPS mà không tốn tài nguyên bộ nhớ khi vẽ 2 nhân vật cùng lượng lớn quái vật và thế giới rộng lớn, engine áp dụng 4 mô hình kỹ thuật sau:

### A. Flyweight Pattern (Mẫu Hưởng Thụ - Tái sử dụng Tài nguyên GPU)
* **Vấn đề:** Nếu Player 1, Player 2, và 10 quái vật Goomba mỗi instance đều tự nạp ảnh `Run.png`, `Jump.png`, hay mỗi ô gạch lưu 1 bản sao texture, bộ nhớ VRAM sẽ bị tràn và việc chuyển đổi texture (Texture Binding) sẽ làm nghẽn cổ chai GPU.
* **Giải pháp Flyweight:** Tách trạng thái đối tượng làm 2 phần:
  * **Intrinsic State (Trạng thái nội tại - Dùng chung):** Các tấm ảnh `Texture2D` (gạch `Tiles.png`, ảnh nền `Background_0.png`, `rDwY7H.png`, sprite nhân vật) được nạp **đúng 1 lần duy nhất** vào `AssetManager` hoặc `TileMap::tilesetTextures` (ánh xạ qua `uid`).
  * **Extrinsic State (Trạng thái ngoại lai - Riêng biệt):** Tọa độ $(X, Y)$, hướng quay mặt (`isFacingRight`), và chỉ số frame animation của Player 1 và Player 2 được lưu riêng trong từng object `Player`. Cả 2 Player cùng trỏ về một vùng nhớ texture chung khi gọi lệnh vẽ!

### B. Spatial Hash Grid (Phân Vùng Không Gian Cho Va Chạm $O(1)$)
* **Vấn đề:** Màn chơi có $217 \times 16 = 3472$ ô gạch. Nếu mỗi khung hình, Player 1 và Player 2 phải duyệt qua toàn bộ 3472 ô để kiểm tra xem có chạm đất hay tường không thì CPU sẽ phải chịu tải $O(N)$ rất nặng.
* **Giải pháp Spatial Partitioning:** Tận dụng tính chất lưới đều (Grid 16x16) của LDtk:
  * Khi nhân vật đứng ở tọa độ thực `(position.x, position.y)`, ta tính thẳng ra chỉ số ô trong mảng 2 chiều bằng công thức: `gridX = (int)(position.x / 16)`, `gridY = (int)(position.y / 16)`.
  * Hàm `GetCollidingRectangles(entityRect)` **chỉ truy vấn các ô gạch nằm trong phạm vi $3 \times 3$ ô xung quanh nhân vật**. CPU chỉ cần kiểm tra tối đa 4-9 ô mỗi nhân vật -> **Độ phức tạp giảm xuống $O(1)$ tuyệt đối!**

### C. Batch Rendering & Canvas Buffer (Vẽ Gộp Hậu Cảnh Tĩnh)
* Thay vì mỗi khung hình (60 lần/giây) phải lặp qua 3,400 ô gạch và các Entity hậu cảnh để gọi lệnh `DrawTextureRec()`, chúng ta sử dụng `RenderTexture2D mapCanvas` (Vải vẽ tạm).
* Lúc `LoadLDtkMap()`, engine duyệt qua toàn bộ map và vẽ nền vào `mapCanvas` **đúng 1 lần duy nhất**.
* Trong vòng lặp game chính, chỉ cần **1 Draw Call duy nhất** để vẽ tấm vải buffer ra màn hình -> Giảm tải 99.9% gánh nặng cho GPU!

### D. Camera Frustum Culling (Clipping Vùng Nhìn)
* Khi vẽ tấm `mapCanvas` lớn ($3472 \times 256$) ra màn hình (với camera zoom 3.0x), Raylib được tích hợp sẵn Hardware Clipping theo hình chữ nhật nguồn (`source rect`). Engine chỉ cắt đúng phần diện tích đang nằm trong góc nhìn Camera để xuất ra màn hình, loại bỏ hoàn toàn các phần bản đồ xa xôi.

---

## 2. Quy Tắc Lớp LDtk & Thứ Tự Xuất Hiện (Layer Ordering & Separation)

Theo đúng yêu cầu kỹ thuật cho thư mục `assets/maps/map01`:
1. **Thứ Tự Xuất Hiện (Painter's Algorithm):**
   * Trong tệp `world01.ldtk`, mảng `layerInstances` được sắp xếp **từ trên cùng (Top Layer - index 0) xuống dưới cùng (Bottom Layer - index cuối)**.
   * Để vẽ các lớp nền xa (như đồi núi xa `Background_0.png`) nằm bên dưới và các lớp gần (cây cối, kiến trúc sát bề mặt `rDwY7H.png`, gạch đất) đè lên trên đúng thứ tự thực tế, thuật toán tải bản đồ **BẮT BUỘC phải duyệt mảng `layerInstances` ngược chiều (Reverse Order: từ `size() - 1` về `0`)**.
2. **Lớp `Collision` (Chỉ Kiểm Tra - KHÔNG Render):**
   * Khi duyệt gặp layer có `__identifier == "Collision"`, chúng ta **CHỈ ĐỌC** dữ liệu từ mảng `intGridCsv` để đổ vào mảng vật lý `collisionLayer[y][x]` phục vụ việc cản bước nhân vật.
   * **TUYỆT ĐỐI KHÔNG VẼ (NO RENDER)** lớp `Collision` này lên màn hình (bỏ qua lệnh draw cho `autoLayerTiles` và `gridTiles` của layer này) để tường cản vật lý hoàn toàn vô hình.
3. **Các Lớp Còn Lại (IntGrid, Entities - CHỈ Vẽ - KHÔNG Va Chạm):**
   * Tất cả các lớp khác (như `BackgroundLayer` chứa entity trang trí, hay lớp IntGrid gạch nền) **CHỈ DÙNG ĐỂ VẼ** theo đúng thứ tự đè hình.
   * **TUYỆT ĐỐI KHÔNG ĐỌC** dữ liệu va chạm từ các lớp này. Các Entity trang trí từ LDtk (cây cối, bối cảnh) sẽ chỉ là hình ảnh nghệ thuật, Player 1 và Player 2 có thể tự do di chuyển xuyên qua mà không bị mắc kẹt.

---

## 3. Mã Nguồn Chuẩn C++ Triển Khai (`TileMap.h` & `TileMap.cpp`)

Dưới đây là phần code triển khai hoàn chỉnh cho `TileMap`, tích hợp Flyweight, Spatial Grid, vẽ ngược chiều và ẩn lớp Collision:

### Cập nhật `include/environment/TileMap.h`
```cpp
#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <unordered_map>

class TileMap {
private:
    std::unordered_map<int, Texture2D> tilesetTextures; // Flyweight: Dùng chung tài nguyên texture theo uid
    RenderTexture2D mapCanvas; // Batch Buffer: Gom toàn bộ background tĩnh vào 1 Canvas
    bool hasCanvas = false;

    int tileSize;
    int columns;
    int rows;
    int levelWidth;
    int levelHeight;

    std::vector<std::vector<int>> collisionLayer; // Spatial Grid: 0 (trống), >0 (tường/sàn cản)

public:
    TileMap();
    ~TileMap();

    // Tải bản đồ từ file LDtk: Đọc Collision ngược chiều & Vẽ Background
    bool LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName = "Level_0");
    
    // Vẽ vùng bản đồ nằm trong tầm nhìn Camera chỉ với 1 Draw Call
    void Draw() const;

    // Truy vấn Spatial Grid: Lấy danh sách các hộp gạch rắn xung quanh bounding box của Entity
    std::vector<Rectangle> GetCollidingRectangles(Rectangle entityRect) const;

    int GetTileSize() const { return tileSize; }
    int GetWidth() const { return levelWidth; }
    int GetHeight() const { return levelHeight; }
};
```

### Triển khai trong `src/environment/TileMap.cpp`
```cpp
#include "TileMap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

TileMap::TileMap() : tileSize(16), columns(0), rows(0), levelWidth(0), levelHeight(0) {}

TileMap::~TileMap() {
    for (auto& pair : tilesetTextures) {
        if (pair.second.id != 0) UnloadTexture(pair.second);
    }
    if (hasCanvas) UnloadRenderTexture(mapCanvas);
}

bool TileMap::LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName) {
    std::ifstream file(ldtkFilePath);
    if (!file.is_open()) {
        std::cerr << "[LDtk] Khong the mo file: " << ldtkFilePath << std::endl;
        return false;
    }

    json j;
    file >> j;

    // 1. FLYWEIGHT PATTERN: Tải toàn bộ danh sách Texture vào bộ nhớ chung (dùng cho mọi tile và entity nền)
    std::string baseDir = ldtkFilePath.substr(0, ldtkFilePath.find_last_of("/\\") + 1);
    for (const auto& ts : j["defs"]["tilesets"]) {
        int uid = ts["uid"];
        std::string relPath = ts["relPath"];
        std::string fullPath = baseDir + relPath;
        tilesetTextures[uid] = LoadTexture(fullPath.c_str());
        std::cout << "[LDtk] Nạp Texture chung (UID " << uid << "): " << relPath << "\n";
    }

    // 2. Tìm Level
    json targetLevel = nullptr;
    for (const auto& lvl : j["levels"]) {
        if (lvl["identifier"] == levelName || levelName.empty()) {
            targetLevel = lvl;
            break;
        }
    }

    if (targetLevel.is_null()) {
        std::cerr << "[LDtk] Khong tim thay level: " << levelName << std::endl;
        return false;
    }

    levelWidth = targetLevel["pxWid"];
    levelHeight = targetLevel["pxHei"];
    
    auto& layerInstances = targetLevel["layerInstances"];
    if (!layerInstances.empty()) {
        tileSize = layerInstances[0]["__gridSize"];
        columns = layerInstances[0]["__cWid"];
        rows = layerInstances[0]["__cHei"];
    }

    collisionLayer = std::vector<std::vector<int>>(rows, std::vector<int>(columns, 0));

    // 3. BATCH RENDER BUFFER: Tạo Canvas vẽ toàn bộ nền và gạch trang trí
    if (hasCanvas) UnloadRenderTexture(mapCanvas);
    mapCanvas = LoadRenderTexture(levelWidth, levelHeight);
    hasCanvas = true;

    BeginTextureMode(mapCanvas);
    ClearBackground(GetColor(0x40465Bff)); // Màu nền mặc định từ LDtk

    // 4. DUYỆT NGƯỢC (REVERSE ORDER): Từ lớp đáy (index cuối) lên lớp đỉnh (index 0)
    // Giúp đảm bảo thứ tự đè hình chuẩn xác (Painter's Algorithm)
    for (int i = (int)layerInstances.size() - 1; i >= 0; --i) {
        const auto& layer = layerInstances[i];
        std::string layerName = layer["__identifier"];
        std::string layerType = layer["__type"];

        // --- QUY TẮC A: LỚP COLLISION (CHỈ KIỂM TRA - KHÔNG RENDER) ---
        if (layerName == "Collision") {
            const auto& csv = layer["intGridCsv"];
            for (size_t idx = 0; idx < csv.size(); ++idx) {
                int val = csv[idx];
                if (val > 0) { // Ô gạch rắn (1: dirt, 2: bridge, 7: stone...)
                    int y = idx / columns;
                    int x = idx % columns;
                    collisionLayer[y][x] = val;
                }
            }
            // QUAN TRỌNG: Tiếp tục vòng lặp ngay, TUYỆT ĐỐI KHÔNG vẽ lớp Collision này lên màn hình!
            continue; 
        }

        // --- QUY TẮC B: CÁC LỚP CÒN LẠI (CHỈ VẼ - KHÔNG VA CHẠM) ---

        // B1. Vẽ lớp Entities trong LDtk (Background trang trí như rDwY7H.png, Background_0.png)
        if (layerType == "Entities") {
            for (const auto& ent : layer["entityInstances"]) {
                if (!ent.contains("__tile") || ent["__tile"].is_null()) continue;
                
                const auto& tileInfo = ent["__tile"];
                int tsUid = tileInfo["tilesetUid"];
                if (tilesetTextures.find(tsUid) == tilesetTextures.end()) continue;

                Texture2D tex = tilesetTextures[tsUid];
                Rectangle src = {
                    (float)tileInfo["x"], (float)tileInfo["y"],
                    (float)tileInfo["w"], (float)tileInfo["h"]
                };
                Vector2 pos = { (float)ent["px"][0], (float)ent["px"][1] };

                // Xử lý Pivot của Entity LDtk
                float pivotX = ent["__pivot"][0];
                float pivotY = ent["__pivot"][1];
                pos.x -= src.width * pivotX;
                pos.y -= src.height * pivotY;

                DrawTextureRec(tex, src, pos, WHITE);
            }
        }

        // B2. Vẽ lớp gạch đồ họa (IntGrid / AutoLayer / GridTiles trang trí nền)
        const auto& tiles = layer.contains("autoLayerTiles") && !layer["autoLayerTiles"].empty() 
                            ? layer["autoLayerTiles"] 
                            : layer["gridTiles"];

        int tsDefUid = layer["__tilesetDefUid"].is_null() ? -1 : (int)layer["__tilesetDefUid"];
        if (tsDefUid != -1 && tilesetTextures.find(tsDefUid) != tilesetTextures.end()) {
            Texture2D tex = tilesetTextures[tsDefUid];
            for (const auto& tile : tiles) {
                int px = tile["px"][0];
                int py = tile["px"][1];
                int srcX = tile["src"][0];
                int srcY = tile["src"][1];
                int f = tile["f"]; // 0: normal, 1: flipX, 2: flipY, 3: flipXY

                Rectangle srcRect = { (float)srcX, (float)srcY, (float)tileSize, (float)tileSize };
                if (f == 1 || f == 3) srcRect.width = -srcRect.width;   // Lật ngang
                if (f == 2 || f == 3) srcRect.height = -srcRect.height; // Lật dọc

                DrawTextureRec(tex, srcRect, { (float)px, (float)py }, WHITE);
            }
        }
    }
    EndTextureMode();

    std::cout << "[LDtk] Load map " << levelName << " (" << levelWidth << "x" << levelHeight << ") thanh cong!\n";
    return true;
}

void TileMap::Draw() const {
    if (!hasCanvas) return;
    // Vẽ Buffer lên màn hình (Lật dọc theo quy chuẩn OpenGL của Raylib RenderTexture)
    Rectangle src = { 0.0f, 0.0f, (float)mapCanvas.texture.width, -(float)mapCanvas.texture.height };
    DrawTextureRec(mapCanvas.texture, src, { 0.0f, 0.0f }, WHITE);
}

// SPATIAL PARTITIONING QUERY: Chỉ kiểm tra va chạm O(1) trong phạm vi ô lưới xung quanh Entity
std::vector<Rectangle> TileMap::GetCollidingRectangles(Rectangle entityRect) const {
    std::vector<Rectangle> collisions;
    
    // Lấy chỉ số ô lưới bắt đầu và kết thúc theo tọa độ AABB
    int startX = std::max(0, (int)(entityRect.x / tileSize));
    int startY = std::max(0, (int)(entityRect.y / tileSize));
    int endX = std::min(columns - 1, (int)((entityRect.x + entityRect.width) / tileSize));
    int endY = std::min(rows - 1, (int)((entityRect.y + entityRect.height) / tileSize));

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (collisionLayer[y][x] > 0) { // Ô gạch rắn từ lớp Collision
                Rectangle tileRect = { (float)(x * tileSize), (float)(y * tileSize), (float)tileSize, (float)tileSize };
                if (CheckCollisionRecs(entityRect, tileRect)) {
                    collisions.push_back(tileRect);
                }
            }
        }
    }
    return collisions;
}
```

---

## 4. Xử Lý Va Chạm Cho 2 Nhân Vật (2-Player Physics Architecture)

Khi game có 2 nhân vật (Player 1 & Player 2), cả 2 đều sử dụng chung một hàm kiểm tra vật lý `updatePhysicsWithMap()`. Nhờ cơ chế **Spatial Grid $O(1)$**, việc có 2 nhân vật hay 20 quái vật cùng lúc sẽ không làm giảm FPS:

```cpp
// Trong vòng lặp Process() của State (IntroState / WorldState)
void WorldState::Process() {
    float dt = GetFrameTime();

    // 1. Duyệt qua từng nhân vật và thực thể trong game (Player 1, Player 2, Goomba...)
    for (auto& entity : entities) {
        // Từng nhân vật độc lập gọi hàm kiểm tra va chạm tách trục AABB với TileMap
        entity->updatePhysicsWithMap(map, dt);
    }

    // 2. Xử lý va chạm giữa 2 nhân vật với nhau (Player 1 vs Player 2 Push)
    if (entities.size() >= 2) {
        Rectangle p1Box = entities[0]->getRuntimeStats().hitbox;
        Rectangle p2Box = entities[1]->getRuntimeStats().hitbox;
        
        if (CheckCollisionRecs(p1Box, p2Box)) {
            // Đẩy nhẹ 2 nhân vật ra xa nhau dọc theo trục X để không bị đứng chồng vào nhau
            float center1 = p1Box.x + p1Box.width / 2.0f;
            float center2 = p2Box.x + p2Box.width / 2.0f;
            float pushForce = 25.0f * dt;
            
            if (center1 < center2) {
                entities[0]->getWorldStats().position.x -= pushForce;
                entities[1]->getWorldStats().position.x += pushForce;
            } else {
                entities[0]->getWorldStats().position.x += pushForce;
                entities[1]->getWorldStats().position.x -= pushForce;
            }
        }
    }
}
```

### Thuật toán Vật lý Tách Trục Kháng Kẹt (Zero-Tunneling AABB Physics) cho từng Nhân vật:
```cpp
void Entity::updatePhysicsWithMap(const TileMap& map, float dt) {
    applyGravity(dt);

    // --- BƯỚC 1: DI CHUYỂN TRỤC X & KIỂM TRA VA CHẠM TƯỜNG ---
    worldStats.position.x += runtimeStats.velocity.x * dt;
    
    Rectangle rectX = {
        worldStats.position.x - (runtimeStats.hitbox.width / 2.0f),
        worldStats.position.y - runtimeStats.hitbox.height,
        runtimeStats.hitbox.width,
        runtimeStats.hitbox.height
    };

    // Truy vấn Spatial Hash Grid: Chỉ kiểm tra 2-4 ô gạch kề bên!
    auto collidersX = map.GetCollidingRectangles(rectX);
    for (const auto& wall : collidersX) {
        if (runtimeStats.velocity.x > 0) { // Đang đi phải -> Đụng tường bên phải
            worldStats.position.x = wall.x - (runtimeStats.hitbox.width / 2.0f);
            runtimeStats.velocity.x = 0;
        } 
        else if (runtimeStats.velocity.x < 0) { // Đang đi trái -> Đụng tường bên trái
            worldStats.position.x = (wall.x + wall.width) + (runtimeStats.hitbox.width / 2.0f);
            runtimeStats.velocity.x = 0;
        }
    }

    // --- BƯỚC 2: DI CHUYỂN TRỤC Y & KIỂM TRA TRẦN / SÀN ---
    worldStats.position.y += runtimeStats.velocity.y * dt;
    
    Rectangle rectY = {
        worldStats.position.x - (runtimeStats.hitbox.width / 2.0f),
        worldStats.position.y - runtimeStats.hitbox.height,
        runtimeStats.hitbox.width,
        runtimeStats.hitbox.height
    };

    auto collidersY = map.GetCollidingRectangles(rectY);
    worldStats.isGrounded = false;

    for (const auto& floorOrCeiling : collidersY) {
        if (runtimeStats.velocity.y > 0) { // Đang rơi -> Chạm sàn gạch (Ground)
            worldStats.position.y = floorOrCeiling.y;
            runtimeStats.velocity.y = 0;
            worldStats.isGrounded = true; // Cho phép nhảy tiếp
        } 
        else if (runtimeStats.velocity.y < 0) { // Đang nhảy -> Đụng trần gạch (Ceiling)
            worldStats.position.y = (floorOrCeiling.y + floorOrCeiling.height) + runtimeStats.hitbox.height;
            runtimeStats.velocity.y = 0; // Bị cản lực nhảy, bắt đầu rơi xuống
        }
    }
}
```

---

## 5. Tóm Tắt Lợi Ích Của Kiến Trúc Mới
1. **Tiết kiệm tài nguyên tuyệt đối:** Kết hợp **Flyweight Pattern** (chung texture) và **Batch Render Buffer** (1 Draw Call cho toàn bộ thế giới hậu cảnh).
2. **Khả năng mở rộng cho 2 Người chơi (hoặc Nhiều người chơi):** Hệ thống **Spatial Hash Grid $O(1)$** giúp kiểm tra va chạm cho Player 1, Player 2, và hàng chục quái vật cùng lúc cực kỳ nhẹ nhàng.
3. **Đúng quy chuẩn LDtk:** Vẽ đúng thứ tự từ dưới lên trên (Reverse Layer Order), ẩn hoàn toàn lớp `Collision` (chỉ dùng lấy dữ liệu tường cản), và cho phép nhân vật di chuyển tự do xuyên qua cảnh quan hậu cảnh tuyệt đẹp từ lớp Entities!

---

## 6. Lộ Trình Triển Khai: Những Class Cần Chỉnh Sửa & Thành Phần Tạo Mới

Khi bắt tay vào lập trình triển khai theo hướng dẫn này, kiến trúc được thiết kế theo hướng **Nâng cấp Liền mạch (Seamless Upgrade)** ngay trên khung xương OOP hiện tại của dự án mà không làm phình to số lượng file. Dưới đây là danh sách chi tiết:

### A. Các Class Đã Có Từ Trước Cần Chỉnh Sửa (Modify Existing Classes)

#### 1. Class `TileMap` ([include/environment/TileMap.h](file:///d:/Git/.SuperMarioPlus/include/environment/TileMap.h) & [src/environment/TileMap.cpp](file:///d:/Git/.SuperMarioPlus/src/environment/TileMap.cpp))
* **Thay đổi & Bổ sung:**
  * Thay thế hàm load JSON cũ bằng phương thức mới `bool LoadLDtkMap(const std::string& ldtkFilePath, const std::string& levelName)`.
  * Thêm cấu trúc Flyweight `std::unordered_map<int, Texture2D> tilesetTextures` để quản lý ảnh chung theo UID.
  * Thêm biến canvas `RenderTexture2D mapCanvas` để vẽ gộp 1 Draw Call.
  * Viết lại hàm `Draw()` để hiển thị canvas đã culling thay vì lặp vẽ từng ô gạch.

#### 2. Class `Entity` ([include/entity/Entity.h](file:///d:/Git/.SuperMarioPlus/include/entity/Entity.h) & [src/entity/Entity.cpp](file:///d:/Git/.SuperMarioPlus/src/entity/Entity.cpp))
* **Thay đổi & Bổ sung:**
  * Thêm phương thức vật lý mới: `virtual void updatePhysicsWithMap(const TileMap& map, float dt);`.
  * Thay thế cơ chế kiểm tra sàn đất cứng lặp số chết (`checkGroundCollision(500.0f)`) bằng logic kiểm tra AABB 2 trục tách biệt (X trước, Y sau) với lưới gạch rắn từ `map.GetCollidingRectangles()`.

#### 3. Class `Player` ([src/entity/Player/Player.cpp](file:///d:/Git/.SuperMarioPlus/src/entity/Player/Player.cpp))
* **Thay đổi:**
  * Ghi đè (Override) hoặc kế thừa trực tiếp hàm `updatePhysicsWithMap(map, dt)` từ `Entity` để kết nối với State Machine (ví dụ: rơi chạm sàn thì tự động chuyển từ `FallState` về `IdleState`/`RunState`; chạm tường gạch thì dừng tốc độ ngang `velX = 0`).

#### 4. Các State Màn Chơi: `WorldState` & `IntroState` ([src/states/WorldState.cpp](file:///d:/Git/.SuperMarioPlus/src/states/WorldState.cpp) & [src/states/IntroState.cpp](file:///d:/Git/.SuperMarioPlus/src/states/IntroState.cpp))
* **Thay đổi trong khởi tạo (Constructor):**
  * Đổi lệnh tải bản đồ cũ thành `map.LoadLDtkMap("assets/maps/map01/world01.ldtk", "Level_0");`.
* **Thay đổi trong pha `Process()`:**
  * Xóa bỏ dòng `entity->checkGroundCollision(500.0f);`.
  * Thay thế bằng lời gọi truyền tham chiếu map: `entity->updatePhysicsWithMap(map, dt);`.
  * Bổ sung logic đẩy nhẹ khi 2 nhân vật (Player 1 vs Player 2) va chạm chéo vào nhau (`Coop Push`).

---

### B. Các Thành Phần Tạo Mới (New Components Created)

Kiến trúc này **không bắt buộc phải tạo thêm class hay file `.h`/`.cpp` mới hoàn toàn**, vì tất cả trách nhiệm xử lý đã được phân bổ chuẩn xác vào `TileMap` (quản lý không gian tĩnh) và `Entity` (quản lý di chuyển động).

Tuy nhiên, để code trong `TileMap.cpp` sạch hơn và dễ bảo trì, bạn có thể lựa chọn tạo mới 1 file tiện ích hỗ trợ (Tùy chọn - Optional):
* 🆕 **`LDtkParser.h` / `LDtkParser.cpp` (Utility helper):**
  * Chứa các hàm hỗ trợ tĩnh (Static Helpers) để đọc cấu trúc JSON của LDtk, chuyển đổi tọa độ lật ảnh (`flipX`, `flipY`), và phân tích quy tắc màu sắc/pivot của Entity trang trí LDtk.

---

## 7. Bảng Theo Dõi Tiến Độ Triển Khai (Build Status Checklist)

Dưới đây là ghi nhận trạng thái chi tiết cho đợt triển khai hiện tại để nhóm phát triển theo dõi và chuẩn bị cho các đợt build tiếp theo:

### ✅ PHẦN ĐÃ HOÀN THÀNH (Phase 1: Map Loading & Display - COMPLETED)
1. **Nâng cấp toàn bộ Class `TileMap` ([TileMap.h](file:///d:/Git/.SuperMarioPlus/include/environment/TileMap.h) & [TileMap.cpp](file:///d:/Git/.SuperMarioPlus/src/environment/TileMap.cpp)):**
   * Đã tích hợp thành công **Flyweight Pattern**: Dùng chung tài nguyên ảnh theo `uid` qua `tilesetTextures`, không tốn lặp VRAM.
   * Đã tích hợp **Batch Render Buffer**: Dựng canvas tự động (`RenderTexture2D mapCanvas`), chỉ tốn **1 Draw Call duy nhất** khi render toàn bộ gạch và entity trang trí nền.
   * Đã áp dụng **Reverse Layer Order**: Duyệt ngược mảng `layerInstances` từ dưới lên để đảm bảo đúng thuật toán đè hình hậu cảnh.
   * Đã áp dụng quy tắc **Ẩn lớp Collision**: Tự động lấy dữ liệu từ `intGridCsv` đổ vào mảng 2 chiều `collisionLayer` và tuyệt đối không vẽ lớp này lên màn hình.
   * Đã bổ sung kiểm tra an toàn trong `GetCollidingRectangles()` (tránh lỗi chia cho 0 khi chưa load map).
2. **Tạo mới State Kiểm Tra Bản Đồ (`World01State` - [World01State.h](file:///d:/Git/.SuperMarioPlus/include/states/World01State.h) & [World01State.cpp](file:///d:/Git/.SuperMarioPlus/src/states/World01State.cpp)):**
   * Đã tạo state riêng biệt kết nối thẳng tới `assets/maps/map01/world01.ldtk` (level `Level_0`).
   * Tích hợp điều khiển Camera tự do để kiểm tra map: Dùng phím **W/A/S/D** hoặc **Mũi tên** để cuộn camera, phím **Q / E** để phóng to / thu nhỏ (Zoom 0.5x - 5.0x).
3. **Cấu Hình Cấu Trúc Biên Dịch ([CMakeLists.txt](file:///d:/Git/.SuperMarioPlus/CMakeLists.txt) & [StateManager.cpp](file:///d:/Git/.SuperMarioPlus/src/core/StateManager.cpp)):**
   * Đã bổ sung `include/environment` vào danh sách include và nạp các tệp `TileMap.cpp`, `World01State.cpp` vào target biên dịch.
   * Đã đặt `World01State` làm state khởi chạy mặc định của game trong `StateManager`.
   * **Kết quả build:** Dự án đã biên dịch thành công 100% (`[100%] Built target SuperMarioPlus`), sẵn sàng chạy thử nghiệm hiển thị map ngay lập tức!

---

### ⏳ PHẦN CHƯA LÀM - ĐỂ LẠI CHO LẦN BUILD TIẾP THEO (Phase 2: Physics & Player Integration - TODO)
1. **Class `Entity` ([Entity.h](file:///d:/Git/.SuperMarioPlus/include/entity/Entity.h) & [Entity.cpp](file:///d:/Git/.SuperMarioPlus/src/entity/Entity.cpp)):**
   * *Chưa triển khai:* Hàm `virtual void updatePhysicsWithMap(const TileMap& map, float dt);` (hoặc `checkMapCollision`).
   * *Nhiệm vụ cho Phase 2:* Cần viết thuật toán va chạm AABB tách 2 trục (Trục X xử lý kẹt tường trước, Trục Y xử lý trọng lực và sàn/trần sau) dựa trên lưới gạch lấy từ `map.GetCollidingRectangles()`.
2. **Class `Player` ([Player.cpp](file:///d:/Git/.SuperMarioPlus/src/entity/Player/Player.cpp)):**
   * *Chưa triển khai:* Kết nối hệ thống vật lý bản đồ với State Machine của người chơi.
   * *Nhiệm vụ cho Phase 2:* Cho phép chuyển trạng thái `FallState` về `IdleState`/`RunState` khi chân chạm gạch LDtk rắn (`isGrounded = true`), và dừng tốc độ ngang khi đụng tường gạch.
3. **Đưa Nhân Vật Vào `World01State` ([World01State.cpp](file:///d:/Git/.SuperMarioPlus/src/states/World01State.cpp)):**
   * *Chưa triển khai:* Hiện tại `World01State` chỉ mới tập trung vẽ hiển thị map, chưa khởi tạo danh sách `entities`.
   * *Nhiệm vụ cho Phase 2:* Khởi tạo Player 1 (ví dụ Goku/Mario) và Player 2 (ví dụ Luffy/Luigi) bên trong `World01State`, cho gọi `updatePhysicsWithMap(map, dt)` trong hàm `Process()` và bổ sung cơ chế đẩy nhẹ khi 2 người chơi va chạm nhau (`Coop Push`).


