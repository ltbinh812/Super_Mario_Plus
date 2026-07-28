`# Báo Cáo Quy Hoạch & Thiết Kế Kiến Trúc: Hệ Thống Va Chạm (`Entity` vs `TileMap`)

Tài liệu này báo cáo chi tiết thiết kế kiến trúc xử lý vật lý và va chạm bản đồ (`TileMap`) cho hệ thống thực thể (`Entity`, `Player`, `Enemy`, `Item`) theo các chuẩn mực Lập trình Hướng đối tượng (OOP) và Design Pattern hiện đại, phục vụ trực tiếp cho lộ trình refactor và mở rộng game **Super Mario Plus**.

---

## 1. Tổng Quan & Mục Tiêu Kiến Trúc

### A. Vấn Đề Của Kiến Trúc Cũ
1. **Kiểm tra mặt đất giả định (`checkGroundCollision`):** Các thực thể đang kiểm tra va chạm với một đường thẳng ngang vô hình cố định (ví dụ `groundY = 208` trong Map 1 hoặc `300` trong Map 2). Điều này khiến nhân vật không thể đứng lên các bục gạch, đi qua tường, hoặc rơi xuống hố thực tế của bản đồ.
2. **Gộp chung di chuyển X và Y (`updatePosition`):** Tọa độ X và Y được cộng đồng thời trong một bước. Trong game 2D Platformer, việc này gây lỗi xuyên góc gạch (corner clipping) hoặc dính chặt vào tường khi đang bay/nhảy chéo.
3. **Phân mảnh logic vật lý trong Game State:** Các lớp `World01State` và `World02State` phải gọi lẻ tẻ 4 hàm (`applyGravity`, `updatePosition`, `checkGroundCollision`, `updateStateFromPhysics`) trong bước `Process()`, khiến State phải biết quá sâu vào logic vật lý nội bộ của Entity.

### B. Mục Tiêu Refactor
- Đóng gói toàn bộ thuật toán va chạm AABB vào lớp gốc `Entity` theo mẫu **Template Method Pattern**.
- Giữ vững 100% quy trình 4 bước của Game Engine: `HandleInput` -> `Update` -> `Process` -> `Render`.
- Loại bỏ hoàn toàn sự phụ thuộc vào các đường thẳng mặt đất giả định; tích hợp kiểm tra trực tiếp với khối gạch `16x16px` từ `TileMap::GetCollidingRectangles()`.
- Chừa cổng mở rộng đa hình (Polymorphic Hooks) cho `Enemy`, `Boss`, và `Item` trong tương lai mà không sử dụng enum vi phạm nguyên tắc OOP.

---

## 2. Giải Đáp Chuyên Sâu Về Kiến Trúc OOP

### A. Vì Sao Không Sử Dụng Enum (`EntityType`) Để Định Danh & Phân Nhánh?
* **Vi phạm nguyên tắc OOP:** Khi hệ thống đã thiết lập cấu trúc cây kế thừa (`Entity` -> `Player`, `Enemy`, `Item`), chính **Kiểu Dữ Liệu Lớp (Class Type)** và **Đa Hình (Polymorphism)** đã đóng vai trò định danh.
* **Nguyên tắc Mở/Đóng (OCP):** Việc sử dụng enum kèm theo các lệnh `switch/case` hoặc `if (type == ENEMY)` sẽ buộc lập trình viên phải sửa đổi code ở hàng loạt nơi mỗi khi thêm một lớp thực thể mới.
* **Giải pháp thay thế:** Thay vì dùng enum, lớp gốc `Entity` cung cấp sẵn các cổng đàm phán va chạm ảo (ví dụ: `virtual void onCollide(Entity& other) {}`). Khi hai thực thể chạm nhau, cơ chế **Double Dispatch / Visitor Pattern** sẽ cho phép chúng tự tương tác trực tiếp theo đúng chuẩn OOP.

### B. Chuyển Dịch Từ Các Hàm Helper Lẻ Tẻ Sang Template Method (`updatePhysicsWithMap`)
* Toàn bộ 3 hàm helper cũ ở lớp `Player` (`applyGravity`, `updatePosition`, `checkGroundCollision`) sẽ được **XÓA BỎ**.
* Logic của chúng được chuyển tiếp và nâng cấp thành hàm khung **Template Method** `void updatePhysicsWithMap(const TileMap& map, float dt)` tại lớp cha `Entity`.
* **Lợi ích DRY (Don't Repeat Yourself):** Mọi thực thể trong game (từ Mario, Goomba đến Nấm Super Mushroom) đều tuân theo cùng một định luật trọng lực và va chạm tường gạch. Việc đặt thuật toán này ở lớp gốc giúp toàn bộ các lớp con tương lai thừa hưởng 100% khả năng va chạm bản đồ chính xác mà không cần viết lại dù chỉ một dòng mã kiểm tra AABB.

### C. Vai Trò Của Các Hook Methods Đa Hình Và Trách Nhiệm Của Lớp `Player`
Lớp cha `Entity` chịu trách nhiệm giải quyết bài toán hình học (dừng vận tốc, đẩy hộp va chạm ra khỏi viên gạch), sau đó kích hoạt các **Hook Methods (Hàm Móc Nối)** để lớp con quyết định hành vi đặc thù:

```cpp
virtual void onHitWall(bool isRightWall) {}
virtual void onLand(float floorY) {}
virtual void onHitCeiling(float ceilY) {}
virtual void onCollide(Entity& other) {} // Cổng mở cho tương lai
```

* **Trách nhiệm của `Player`:** Lớp `Player` cần ghi đè `onLand(float floorY)` và `onHitCeiling(float ceilY)`:
  * `onLand(floorY)`: Nhận biết cờ tiếp đất (`isGrounded = true`) đã được lớp cha thiết lập để thông báo cho **State Machine** chuyển từ `FallState`/`JumpState` về `IdleState`/`RunState`.
  * `onHitCeiling(ceilY)`: Cung cấp điểm neo để phát ra hiệu ứng âm thanh cụng gạch hoặc kích hoạt phá gạch/hộp dấu hỏi (Question Block).
* **Sự khác biệt với thực thể tương lai (`Enemy` / `Item`):** Lớp `Enemy` (như Goomba) hay `Item` (như Nấm) khi kế thừa `Entity` sẽ ghi đè hook `onHitWall(bool isRightWall)` để tự động đổi chiều di chuyển (`velocity.x *= -1`), trong khi `Player` chỉ đứng lại khi đụng tường.

---

## 3. Thuật Toán Tách Trục (Axis-Separated AABB Resolution)

Thuật toán trong `Entity::updatePhysicsWithMap(const TileMap& map, float dt)` được thực hiện tuần tự qua 3 bước tuyệt đối không gộp chung:

1. **Bước 1 (Gia tốc trọng lực):** Cộng gia tốc vào vận tốc trục Y: `runtimeStats.velocity.y += baseStats.gravityScale * 9.8f * dt;`
2. **Bước 2 (Di chuyển X & Kiểm tra tường):** Di chuyển tọa độ X -> Kiểm tra va chạm với `TileMap::GetCollidingRectangles()`. Nếu va chạm tường gạch, đẩy tọa độ X sát mép gạch, đặt `velocity.x = 0`, và kích hoạt hook `onHitWall(isRightWall)`.
3. **Bước 3 (Di chuyển Y & Kiểm tra sàn/trần):** Di chuyển tọa độ Y -> Kiểm tra va chạm với `TileMap::GetCollidingRectangles()`. Nếu rơi xuống chạm sàn, đẩy Y lên đỉnh gạch, đặt `velocity.y = 0`, `isGrounded = true`, và kích hoạt hook `onLand(floorY)`. Nếu bay lên chạm trần, đẩy Y đáy gạch, đặt `velocity.y = 0`, và kích hoạt `onHitCeiling(ceilY)`. Nếu không va chạm sàn, kiểm tra thêm 1 pixel dưới chân để phát hiện khi nhân vật đi ra khỏi mép bục gạch (walk off cliff) và chuyển cờ `isGrounded = false`.

---

## 4. Quy Trình Vận Hành Cốt Lõi (Engine Lifecycle)

Trong các lớp State (`World01State`, `World02State`), vòng đời 4 bước được giữ nguyên 100%, bước `Process()` được tinh gọn đạt chuẩn OOP:

1. **`HandleInput()`:** Đọc lệnh từ bàn phím thông qua `InputHandler` và truyền Command cho nhân vật.
2. **`Update(float dt)`:** Cập nhật đếm thời gian, animation và tọa độ camera `MapCamera`.
3. **`Process()`:** Kích hoạt xử lý vật lý và va chạm bản đồ tuân thủ OOP:
   ```cpp
   if (player) {
       float dt = GetFrameTime();
       player->updatePhysicsWithMap(map, dt);
       player->updateStateFromPhysics();
   }
   ```
4. **`Render(float alpha)`:** Vẽ `TileMap`, `Player` và các debug overlays lên màn hình.

---

## 5. Danh Sách Thực Thi Mã Nguồn (Execution Plan)

Các thay đổi kỹ thuật được thực thi trên các file sau:
1. **`include/entity/Entity.h` & `src/entity/Entity.cpp`:** Thêm `updatePhysicsWithMap`, `getHitbox`, và các virtual hook methods (`onLand`, `onHitWall`, `onHitCeiling`, `onCollide`).
2. **`include/entity/Player/Player.h` & `src/entity/Player/Player.cpp`:** Xóa bỏ 3 hàm helper cũ, ghi đè `onLand(float floorY)` và `onHitCeiling(float ceilY)`.
3. **`include/states/World01State.h/.cpp` & `include/states/World02State.h/.cpp`:** Chuẩn hóa tên biến (`testPlayer` -> `player`), cập nhật `Process()` gọi `player->updatePhysicsWithMap(map, dt);`.
