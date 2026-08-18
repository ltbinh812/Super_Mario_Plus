# Tổng Kết Quá Trình Merge: `binh01` + `minh1` -> `binh02`

Tài liệu này tóm tắt toàn bộ quá trình, quyết định kỹ thuật và các thay đổi cốt lõi khi gộp hai nhánh `binh01` (chuyên về Map, Môi trường, Vật lý) và `minh1` (chuyên về Player, Hệ thống Skill, Combat) vào nhánh chung `binh02`.

---

## 1. Phương pháp & Chiến lược Merge
Vì hai nhánh phát triển từ một codebase gốc nhưng đi theo hai hướng chuyên sâu khác nhau:
- **`binh01`**: Tập trung mở rộng hệ thống môi trường, thêm nhiều world (map 1 đến 5), hệ thống vật thể (item, buff), và vật lý tương tác với `TileMap`.
- **`minh1`**: Tập trung nâng cấp toàn diện logic Nhân vật (`Player`), tái cấu trúc `CombatSystem` (không lưu trạng thái - stateless), và xây dựng hệ thống Kỹ năng/Combo đa dạng.

**Chiến lược áp dụng:**
Lấy `binh01` làm nền tảng kiến trúc (do chứa nhiều level và state), sau đó chèn đè/bổ sung các hệ thống logic nhân vật và chiến đấu từ `minh1` sang. Các file xung đột được giải quyết bằng cách kết hợp cơ chế vật lý của `binh01` với cơ chế combat của `minh1`.

---

## 2. Chi tiết phân bổ Codebase

### 🟢 Các thành phần lấy TOÀN BỘ từ `binh01`
*Bao gồm các file xử lý đồ hoạ môi trường, vật phẩm, và luồng chạy của Game State.*
- **Map & Environment:** `TileMap.cpp`, `TileMap.h` và thư mục `assets/maps`.
- **Item & Buff:** Toàn bộ thư mục `src/entity/Item/` và `include/entity/Item/` (`HealthPotion`, `ManaPotion`, ...), hệ thống `BuffManager`.
- **World States:** `World02State`, `World03State`, `World04State`, `World05State` và các file header tương ứng.
- **BaseLevelState (Phần lõi môi trường):** Xử lý load map, sinh item, render map camera, và cập nhật vật lý nhân vật dựa vào địa hình (`dynamicSolids`).

### 🔵 Các thành phần lấy TOÀN BỘ từ `minh1`
*Bao gồm toàn bộ nhân sự và võ công của game.*
- **Nhân vật & Animation:** File cấu hình `characters.json`, tài nguyên nhân vật (`assets/goku/`, `assets/zoro/`...).
- **Hệ thống Player:** `Player.cpp`, `Player.h`, `PlayerFactory.cpp` cùng tất cả các State của người chơi (`PlayerIdleState`, `PlayerRunState`, `PlayerSkillState`, ...).
- **Hệ thống Kỹ năng (Skill):** Toàn bộ thư mục `src/entity/Skill/` và `include/entity/Skill/` (`Attack1Skill`, `DashSkill`, `SpecialSkillAttack`, ...).
- **CombatSystem:** Cấu trúc Combat ưu việt của `minh1` (stateless - không lưu trữ danh sách entity bên trong CombatSystem).

---

## 3. Các File bị Xoá hoặc Đổi Tên
- **Đổi tên thư mục Asset:** Thư mục `Goku_animation` cũ được đổi tên và quy chuẩn lại thành `goku` theo đúng cấu trúc `characters.json` của `minh1`.
- **Xoá hàm trong `BaseLevelState`:**
  - Xoá các hàm `combatSystem.registerEntity()`.
  - Xoá `combatSystem.removeInactive()`.
  - Xoá `combatSystem.clear()`.
  *(Lý do: Nhánh `minh1` đã tái cấu trúc `CombatSystem` không còn lưu trạng thái nội bộ, nên các hàm quản lý vòng đời entity này bị loại bỏ hoàn toàn).*

---

## 4. Các File được Sửa Đổi Kép (Modify từ cả 2 nhánh)

Đây là những file bắt buộc phải trộn logic bằng tay để cả 2 hệ thống tương thích với nhau:

### `src/states/BaseLevelState.cpp`
- **[Giữ từ binh01]**: Giữ nguyên logic cập nhật tọa độ nhân vật với map (`updatePhysicsWithMap`), logic nhặt vật phẩm, và queue sinh quái vật/vật thể (`spawnQueue`).
- **[Giữ từ binh01]**: Cập nhật hàm tạo để hỗ trợ chuỗi chuyển map (truyền `player1Name` từ `World01` lên tới `World05`).
- **[Thêm từ minh1]**: Cập nhật hàm gọi `combatSystem.update(...)` để truyền thẳng mảng `activeEntities` vào mỗi frame (thay vì đăng ký trước).
- **[Thêm từ minh1]**: Thêm cơ chế "Người chơi đẩy nhau" (Player-to-Player Pushing Collision). Nếu 2 người chơi đi xuyên nhau, hệ thống tự động bù trừ tọa độ (overlap) để đẩy họ ra xa, đồng thời check `TileMap` để không đẩy người chơi xuyên tường.
- **[Chỉnh sửa Input]**: Đồng bộ hệ thống phím của `minh1`:
  - Player 2: Đổi bộ nút điều khiển sang Numpad (`KEY_KP_1` -> `KEY_KP_5`).
  - Đổi cơ chế nhận diện phím tung chiêu/nhảy từ **`InputType::DOWN`** (của `binh01`) sang **`InputType::PRESSED`** (của `minh1`) để sửa lỗi combo attack không hoạt động và lỗi spam đòn.

### `include/entity/Explosion.h` & `src/entity/Explosion.cpp`
- **[Sửa đổi tham số]**: Khắc phục lỗi Conflict hàm `takeDamage`. Hàm `takeDamage` được cập nhật lại chữ ký (`int amount, const Vector2& knockback`) để tương thích chuẩn với giao diện Entity trong nhánh `minh1` nhưng vẫn duy trì tác dụng sát thương môi trường của `binh01`.

### `src/states/CharacterSelectionState.cpp`
- **[Sửa đổi]**: Tuỳ chỉnh lại đoạn gọi `ChangeStateCommand` để tự động load vào `World05State` thay vì `World01` hoặc `World02` nhằm phục vụ công tác test toàn diện.

---

## Tổng Kết
Phiên bản `binh02` hiện tại là một bản Merge ổn định (Stable Build). Code đã loại bỏ toàn bộ các phương thức Combat thừa thải, kết hợp hoàn hảo hệ thống Map/Physics vững chắc của `binh01` và hệ thống Skill/Combo cực kỳ linh hoạt của `minh1`. Hệ thống `Numpad` và chuỗi `Combo Attack` (Đánh thường 1-2-3-4) cũng đã được khôi phục nguyên vẹn.
