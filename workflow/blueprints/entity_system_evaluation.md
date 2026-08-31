# Đánh Giá Hệ Thống Entities (Movement, Skills, FSM, OOP)

Dưới đây là đánh giá thẳng thắn, không nói giảm nói tránh về kiến trúc Entities hiện tại, khả năng di chuyển, kết hợp chiêu thức và thiết kế OOP.

## 1. Đánh giá Khả năng Di chuyển & Kết hợp Skill (Movement & Combat)

Hệ thống hiện tại ghép nối lỏng lẻo và có nhiều **lỗ hổng logic nghiêm trọng** khi kết hợp di chuyển và ra chiêu:

- **Lỗi trượt băng (Sliding) khi đánh:** Khi đang chạy (ở `PlayerRunState` có `velocity.x != 0`) và xài skill (chuyển sang `PlayerSkillState`), biến `velocity.x` **không hề bị reset**. Hậu quả: Nhân vật sẽ trượt dài về phía trước với vận tốc chạy trong suốt thời gian tung chiêu (ví dụ đấm `Punch1`). 
- **Liệt phím hoàn toàn (Input Lock):** `PlayerSkillState` kế thừa từ `PlayerState` nhưng không override các hàm `onMove...`. Khi vào State này, mọi input di chuyển bị vô hiệu hóa. Nếu xài skill trên không (nhảy + đấm), bạn không thể điều hướng (air-drift), rơi thẳng tuột rất cứng nhắc.
- **Lỗi Animation Flickering (Giật frame):** Khi skill kết thúc (`timer == 0`), code gọi thẳng `player.requestState(player.idleState);`. Nếu người chơi đang giữ nút chạy hoặc đang ở trên không, nó sẽ giật về Idle trong 1 frame, trước khi hàm `updateStateFromPhysics` giật nó lại về `Run` hoặc `Fall`. Điều này tạo ra lỗi chớp animation rất amatuer.
- **Kết hợp Nhảy + Lướt (Dash):** `DashSkill` set cứng `velocity.x = 600`. Nếu Dash trên không, nó ném nhân vật đi. Nhưng khi hết Dash, nó lại ép về `idleState` (do logic trên), làm nhân vật lơ lửng ở thế đứng im 1 frame giữa không trung trước khi chuyển sang rơi.

## 2. Đánh giá OOP và Design Pattern

Việc áp dụng State Pattern (`PlayerState`) và Strategy Pattern (`ISkill`) là một hướng đi đúng, tuy nhiên cách triển khai đang vi phạm nguyên tắc thiết kế:

- **Anti-pattern của FSM (State Machine):** Hàm `updateStateFromPhysics()` nằm chình ình trong `Player.cpp` và chạy mỗi frame để *ép* state thay đổi từ bên ngoài. Đây là sự pha trộn tồi tệ giữa FSM và code thủ tục (procedural). Đúng ra, **mỗi State phải tự quyết định khi nào nó kết thúc**. (Ví dụ: `PlayerFallState` trong hàm `update()` tự check `isGrounded` để chuyển sang Idle/Run). Cách làm hiện tại phải vá víu bằng hàm `canExit()` rất rườm rà.
- **Code Smell `const_cast` nguy hiểm:** Trong `Player::getActiveHitbox() const`, bạn ép kiểu `const_cast<Entity*>(static_cast<const Entity*>(this))`. Trả về một pointer có thể thay đổi (`Entity*`) từ một hàm `const` là vi phạm const-correctness. Dễ gây lỗi memory corruption ở Combat System sau này.
- **Tính đóng gói (Encapsulation) kém:** `PlayerState` là friend class của `Player`, gọi các hàm helper của `Player` thoải mái. Điều này tạm chấp nhận, nhưng `PlayerSkillState` lại nắm giữ trực tiếp raw pointer (`ISkill* nextSkill`) của resource quản lý bởi `unique_ptr` trong Player. Nếu `skillList` bị clear, game sẽ crash lập tức (Dangling Pointer).

## 3. Khả năng Tối ưu để Phát triển (Scalability)

Code hiện tại chưa sẵn sàng để scale lên nhiều nhân vật hoặc cơ chế phức tạp:
- **Tra cứu Skill/Animation bằng String:** Trong combat, tính năng combo (`nextComboSkillName`) dùng `std::string` để lookup vào `std::unordered_map` mỗi lần nhấn nút. Nếu game chạy 60FPS và spam nút, nó sẽ tốn CPU vô ích.
- **Hitbox Tick Check:** `PlayerSkillState::isHitboxActive()` gọi hàm lấy `getElapsedTime()` liên tục trong mỗi frame update của `CombatSystem`.

---

## 4. CÁCH KHẮC PHỤC & TỐI ƯU (Implementation Plan)

Để giải quyết triệt để, cần cấu trúc lại logic State và Skill như sau:

### A. Sửa lỗi Di chuyển & Trượt băng
- Trong `ISkill`, thêm thuộc tính `bool stopsMovement` và `bool allowsAirControl`. Nếu skill này là đánh đứng im (Punch), khi `PlayerSkillState::onEnter` chạy, nó đọc thuộc tính này và gọi `player.idle()` (reset `velocity.x = 0`).
- Override `onMoveLeft`/`onMoveRight` trong `PlayerSkillState` để cho phép air-drift nếu skill đó cho phép (dựa vào `allowsAirControl`).

### B. Dọn dẹp Anti-Pattern FSM
- **Xóa bỏ** việc gọi `updateStateFromPhysics()` mỗi frame. Thay vào đó, gán logic kiểm tra vật lý vào bên trong hàm `update()` của từng State (hoặc một hàm `evaluateStateTransition()` riêng của Player).
  - `PlayerIdleState::update(dt)`: Nếu `!isGrounded`, tự động gọi `changeState(fallState)`.
  - `PlayerFallState::update(dt)`: Nếu `isGrounded`, kiểm tra `velocity.x` để nhảy về `Idle` hoặc `Run`.
- Khi `PlayerSkillState` kết thúc (`timer == 0`), không gọi `idleState` mù quáng. Cần kiểm tra trạng thái vật lý và input hiện tại để chuyển tiếp mượt mà.

### C. Khắc phục Tối ưu & Code Smell
- Sửa lại hàm `getActiveHitbox()`: Không dùng `const_cast`. Đảm bảo luồng dữ liệu an toàn bằng cách thay đổi kiểu trả về hoặc cấu trúc Hitbox.
- Cache con trỏ của Next Combo Skill ngay khi Load skill thay vì lookup bằng chuỗi mỗi khi nhấn nút đánh.
