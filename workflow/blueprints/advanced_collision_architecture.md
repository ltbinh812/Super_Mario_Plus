# Hướng dẫn & Kế hoạch triển khai: Hệ thống Vật lý và Va chạm (Advanced Physics & Collision) cho Đa thực thể (Multi-entity)

Tài liệu này trình bày quy chuẩn kiến trúc để tích hợp các block va chạm nâng cao từ LDtk (Solid, OneWay, Ladder, Water, Hazard, Die, Lotus) vào framework của `SuperMarioPlus`.

## 1. Phân tích bài toán & Nguyên tắc OOP

Hiện tại, các layer `Collision` trong `world01.ldtk`, `world02.ldtk`, `world03.ldtk` đang gán ID số nguyên (value) khác nhau cho cùng một loại block (VD: `Water` có ID = 6 ở map01, nhưng là 5 ở map02).
Ngoài ra, class `Entity` hiện mới chỉ xử lý AABB cơ bản cho tường cứng (Solid), chưa có các vật liệu OneWay, Ladder, Water, v.v.

### Giải pháp OOP & Design Pattern:
1. **Enum Mapping (Adapter / Data Transformation):** Tại lúc đọc file LDtk (`TileMap::LoadLDtkMap`), chúng ta sẽ phân tích cấu trúc `defs.layers` để tạo từ điển ánh xạ linh hoạt từ `ID số nguyên -> Chuỗi Identifier (ví dụ "Water") -> Enum CollisionType`.
2. **Bitmask Collision Filtering (Thay thế boolean cờ):** Việc sử dụng nhiều biến `bool` để check trạng thái vật lý (`inWater`, `onLadder`, `intentDropThrough`) là dấu hiệu của **Anti-Pattern (State Explosion)**, dẫn đến code rối rắm (Spaghetti Code) với vô số lệnh `if/else`.
   - Thay vào đó, ta sử dụng **Collision Mask (Mặt nạ va chạm)** bằng một biến `uint32_t collisionMask` (4 bytes bộ nhớ) hoặc `uint64_t` (8 bytes) nếu game cực kì phức tạp. Một biến 32-bit cho phép định nghĩa tới **32 loại bề mặt vật lý khác nhau** (Solid, OneWay, Ice, Mud, Lava, Bounce...). Trong game 2D thông thường, 32 loại là quá dư dả (Mario chỉ có khoảng 7-8 loại). 
   - `collisionMask` quyết định Entity có va chạm với loại block nào. Khi muốn lọt qua `OneWay`, ta chỉ cần gỡ bit `OneWay` ra khỏi mask (`mask &= ~(1 << OneWay)`). Mọi logic check va chạm với OneWay sẽ tự động bị bỏ qua ở cấp độ cội nguồn.
3. **Luồng Tương tác Thực thể (Entity Interaction Flow):**
   - **Tương tác Vật lý (Cản bước):** Ở hàm `updatePhysicsWithMap`, Entity quét các tile xung quanh. Nếu tile đó có trong `collisionMask`, Entity sẽ bị cản lại (đối với Solid) hoặc đứng lên (đối với OneWay).
   - **Tương tác Môi trường (Triggers):** Đối với Water hay Ladder, chúng không cản đường mà là *Vùng kích hoạt (Trigger)*. Khi Entity quét trúng tile Water, nó không check mask cản đường mà bắn ra một tín hiệu Hook: `virtual void onEnterWater()`.
   - **Tiếp nhận bằng State Pattern:** Ở class `Player` (kế thừa Entity), ta hứng Hook `onEnterWater()` và ra lệnh cho State Machine chuyển sang `SwimState`. Lớp `SwimState` sẽ tự lo việc thay đổi trọng lực, bơi lội và animation, giúp `Entity` sạch sẽ tuyệt đối, không có một lệnh `if (inWater)` nào.



---

## 2. Chi Tiết Thay Đổi Mã Nguồn

### A. Lớp `TileMap`

#### [NEW] `CollisionType` (Định nghĩa chung)
Tạo Enum `CollisionType` để đồng nhất ngữ nghĩa:
```cpp
enum class CollisionType {
    None = 0, Solid, OneWay, Hazard, Ladder, Water, Die, Lotus
};
```

#### Cấu trúc truy vấn mới
Bổ sung cấu trúc trả về thay vì chỉ trả về `Rectangle` chung chung:
```cpp
struct CollisionTile {
    Rectangle rect;
    CollisionType type;
};
std::vector<CollisionTile> GetCollidingTiles(Rectangle entityRect) const;
```

#### Xử lý phân tích JSON (TileMap.cpp)
Trong `LoadLDtkMap`, lấy mảng định nghĩa IntGrid của layer "Collision" từ `j["defs"]["layers"]`. Xây dựng map `int -> CollisionType` dựa trên thuộc tính chuỗi `identifier` ("Solid", "OneWay", "Ladder", "Water", "Die", "hoa_sen", "Hazard").

---

### B. Lớp cơ sở `Entity` & `CharacterStats`

#### Áp dụng Bitmask (Loại bỏ boolean lộn xộn)
Trong `CharacterRuntimeStats`:
- Thêm `uint32_t collisionMask = 0xFFFFFFFF;` (Mặc định xét va chạm với tất cả).
- Thêm cơ chế đếm lùi: `float ignoreOneWayTimer = 0.0f;` để tạm thời vô hiệu hóa va chạm `OneWay` khi tụt xuống.
*(Tuyệt đối không dùng các cờ bool như `intentDropThrough`, `intentClimb`, `inWater`, `onLadder`)*

#### Nâng cấp hàm `updatePhysicsWithMap(map, dt)`
Thay vì dùng `if/else` thủ công, luồng xử lý sẽ là:
1. **Filter theo Mask:** Khi lấy danh sách `GetCollidingTiles`, nếu `( (1 << (int)tile.type) & collisionMask ) == 0`, bỏ qua hoàn toàn tile đó.
2. **Overlap check (Trigger môi trường):** Nếu chạm `Water`, gọi hook `virtual void onEnterWater()`. Nếu chạm `Ladder`, gọi hook `virtual void onOverlapLadder()`. Chạm bẫy gọi `onHazard()`.
3. **Trục Y (Floor/Ceiling check):**
   - Khối `Solid`: Cản rơi và cản nhảy.
   - Khối `OneWay` / `Lotus`: Cản rơi **CHỈ KHI** `velocity.y > 0` (đang rơi) **VÀ** tọa độ đáy cũ cao hơn mép trên Tile.

Thêm các hàm API chuyên nghiệp:
```cpp
virtual void dropThrough(); // Thực hiện: loại bỏ OneWay khỏi collisionMask trong 0.2s
virtual void onEnterWater() {}
virtual void onOverlapLadder() {}
```


Thêm các hàm hook ảo (virtual):
```cpp
virtual void onDie() {}
virtual void onHazard() {}
```

---

### C. Lớp `Player` (Subclass) & Command Pattern

- **Tạo các Class Command mới:** `DropThroughCommand`, `ClimbCommand`.
- Khi người chơi bấm phím XUỐNG, `InputHandler` gọi `DropThroughCommand`. Lệnh này sẽ kích hoạt API `entity->dropThrough()` (tạm bỏ OneWay mask).
- Khi người chơi bấm LÊN/XUỐNG tại khu vực cầu thang (`onOverlapLadder()` trả về true), `ClimbCommand` sẽ yêu cầu Player chuyển State machine sang `ClimbState`.
- Khi hook `onEnterWater()` được kích hoạt, Player State machine chuyển sang `SwimState`. Ở State này, trọng lực được thay đổi cục bộ và hoạt ảnh bơi được kích hoạt.

---

### D. Danh sách các File sẽ bị tác động
1. **Include/Entity/CharacterStats.h:** Thêm `collisionMask`, `ignoreOneWayTimer`.
2. **Include/Entity/Entity.h & .cpp:** Thêm các hàm hook (`onEnterWater`, `dropThrough`), logic tính toán va chạm kết hợp Bitmask.
3. **Include/Environment/TileMap.h & .cpp:** Định nghĩa `Enum CollisionType`, `GetCollidingTiles`.
4. **Include/Entity/Player/Player.h & State Classes:** Thêm `SwimState`, `ClimbState`.
5. **Include/Entity/Command/PlayerCommands.h:** Thêm các struct `DropThroughCommand`, `ClimbCommand`.


## 3. Plan Xác Minh (Verification)
1. Bật map03: Leo lên các tảng đá (Solid), nhảy lên mây (OneWay), bấm XUỐNG để rớt qua mây.
2. Kiểm tra phần nước (Water): Rơi vào nước sẽ rơi chậm hơn, bơi được.
3. Chạm bẫy gai (Hazard/Die): Nhân vật gọi hàm Die và reset.
