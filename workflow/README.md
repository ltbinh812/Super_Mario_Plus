# Super Mario Plus - Architecture Blueprints

Thư mục `workflow/` là nơi chứa toàn bộ tài liệu thiết kế kiến trúc của dự án. Gần đây, hệ thống đã được quyết định thiết kế lại (Refactor) để đảm bảo tuân thủ triệt để các Design Pattern chuẩn mực và dọn đường cho việc nâng cấp quy mô cực lớn về sau.

Mọi bản thảo thiết kế chi tiết bằng PlantUML (PUML) đều được đặt trong thư mục `workflow/blueprints/`. 

Dưới đây là mục lục và nội dung giới thiệu chung về các bản vẽ thiết kế:

## Danh sách Blueprints

### 1. `master_game_lifecycle.puml` (Bản đồ tổng quát)
Sơ đồ quan trọng nhất của toàn bộ dự án, đóng vai trò như một "bản đồ dòng chảy":
- Phác họa toàn bộ vòng đời của 1 khung hình (Game Loop Frame) từ khi bắt đầu vòng lặp `while` cho tới kết thúc.
- Trình bày tuần tự 5 giai đoạn chính: `HandleInput` -> `Update` (Fixed-Timestep) -> `ProcessCommands` -> `Event Publish` -> `Render` nội suy.
- Thể hiện sự tương tác theo lớp lang giữa `Game`, `StateManager`, `WorldState`, `Entity` và `Raylib`. Đọc hiểu sơ đồ này là nắm được 100% cách game hoạt động.

### 2. `core_engine.puml`
Sơ đồ trình bày kiến trúc cốt lõi nhất của game:
- Cách hoạt động của Vòng lặp Game (Game Loop) với Fixed-Timestep để giữ ổn định tốc độ vật lý.
- Cấu trúc `StateManager` sử dụng Stack để quản lý các màn hình đồ họa (Intro, Menu, World).

### 3. `command_pattern.puml`
Sơ đồ áp dụng Command Pattern:
- Định nghĩa luồng dữ liệu (Data Flow) khi người dùng bấm phím hoặc click chuột.
- Thay vì gọi lệnh trực tiếp, Input sẽ sinh ra các Object Command (`JumpCommand`, `MoveCommand`) bỏ vào Queue, sau đó được xử lý tập trung. Đây là tiền đề cho chức năng Replay hay Keybinding.

### 4. `entities_architecture.puml`
Sơ đồ phân cấp thực thể (Domain Entities):
- Mối liên kết kế thừa từ gốc `Entity` rẽ nhánh xuống `Character`, `Player`, `Enemy`, `Item`.
- Chỉ ra cách một nhân vật cụ thể (như `Mario` hay `Goomba`) kế thừa từ bộ khung chung như thế nào.

### 5. `player_state_strategy.puml`
Sơ đồ trạng thái (FSM) và Chiến lược (Strategy) cho nhân vật:
- Tách bạch logic hoạt ảnh và di chuyển (Idle, Running, Jumping) thành các class State riêng biệt.
- Khả năng "tháo lắp" kỹ năng động (Fireball, Dash) nhờ Strategy Pattern, tránh việc dùng if-else chằng chịt trong code nhân vật.

### 6. `event_bus.puml`
Sơ đồ sự kiện Observer Pattern:
- Xây dựng trung tâm điều phối tin nhắn `EventBus`.
- Giúp tách rời (decouple) logic Gameplay khỏi UI/Audio (Ví dụ: Kẻ địch chết tự động báo HUD tăng điểm mà không cần biết file HUD.h ở đâu).

### 7. `world_and_collision.puml`
Sơ đồ Hệ thống Màn chơi và Va chạm:
- Giới thiệu cách `LevelLoader` đọc dữ liệu map từ JSON (như Tiled).
- Khái niệm hệ thống `TileMap` không gian lưới (Grid), giúp thuật toán tính toán va chạm siêu nhanh $O(1)$ thay vì duyệt toàn bộ mảng.

### 8. `object_pool.puml`
Sơ đồ tối ưu bộ nhớ Memory Pool:
- Chiến thuật tái sử dụng object (đạn lửa, mảnh vỡ) thay vì gọi `new`/`delete` liên tục.
- Giúp giảm thiểu lỗi phân mảnh bộ nhớ và rớt FPS do Garbage Collection.

---

**Cách sử dụng:**
Hãy mở các file `.puml` trong IDE của bạn (sử dụng extension PlantUML) để xem sơ đồ đồ họa trực quan. Bạn có thể dùng các sơ đồ này như một "bản thiết kế thi công" để tự tay xây dựng và cấu trúc lại toàn bộ mã nguồn C++.