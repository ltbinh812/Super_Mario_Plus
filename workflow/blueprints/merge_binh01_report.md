# Báo Cáo Tích Hợp Nhánh `binh01` và Tối Ưu Hệ Thống Combat/Skill

**Ngày thực hiện:** 07/08/2026
**Mục tiêu:** Tích hợp nhánh `binh01` (chứa các cập nhật về Camera và Map LDTK) vào nhánh hiện tại (chứa logic Combat, Hitbox, Skill), đồng thời sửa các lỗi phát sinh trong quá trình vận hành để đảm bảo chuẩn OOP và thuận tiện phát triển trong tương lai.

---

## 1. Phân Chia Logic Code (Quyết định Merge)

Để giải quyết conflict và đảm bảo tính đóng gói (Encapsulation), chúng ta đã thống nhất quy tắc lấy code như sau:
- **Từ nhánh `binh01` (Sắp merge):** Giữ toàn bộ logic liên quan đến thao tác Map (load LDTK), Camera bám theo nhân vật, xử lý môi trường.
- **Từ nhánh hiện tại (Combat):** Giữ nguyên kiến trúc của hệ thống Skill (`ISkill`, `DashSkill`, `BlockSkill`,...), logic xử lý Hitbox, và hệ thống State của Player (`PlayerSkillState`, `PlayerCrouchState`,...).
- **Cân nhắc/Sửa đổi:** Các hàm làm thay đổi class `Entity` hoặc `Player` được viết lại để vừa tương thích với cơ chế Map mới, vừa giữ được sự mượt mà của hệ thống chiến đấu.

---

## 2. Các File Bị Modify & Fix Lỗi Chi Tiết

Dưới đây là danh sách toàn bộ các thành phần đã được chỉnh sửa và nâng cấp trong hệ thống Combat/State sau khi merge:

### 2.1. Sửa Lỗi Kẹt Trạng Thái Ngồi (Crouch Bug)
- **Files Modified:** `World01State.cpp`, `World02State.cpp`, `World03State.cpp`, `PlayerCrouchState.cpp`
- **Thay đổi:**
  - Phát hiện thiếu Binding cho lệnh nhả phím ngồi (`StopCrouchCommand`). Đã bổ sung đầy đủ phím `S` (Player 1) và `Down` (Player 2) trên toàn bộ các World.
  - Sửa lỗi đánh vần sai tên chiêu tấn công dưới thấp trong `PlayerCrouchState::onAttack` (từ `"low_kick"` thành `"LowKick"` để match với `characters.json`).
  - Bổ sung lệnh ép Hitbox thu nhỏ về `24x32` bằng cách gọi `player.crouch()` ở `onEnter` và khôi phục hitbox bằng `player.idle()` ở `onExit`.

### 2.2. Hoàn Thiện Kỹ Năng Lướt (Dash Skill Hitbox & Stutter Fix)
- **Files Modified:** `DashSkill.cpp`
- **Thay đổi:**
  - Yêu cầu lướt phải qua được các khe hẹp: Bổ sung lệnh `player.crouch()` để sử dụng Hitbox nhỏ trong quá trình lướt.
  - Sửa lỗi giật/khựng khung hình khi lướt: Do hàm `player.crouch()` mặc định ép vận tốc ngang về `0`, nó đã triệt tiêu lực lướt. Đã đảo ngược thứ tự gọi hàm: Gọi `crouch()` trước để thu nhỏ hitbox, sau đó mới gọi `player.dash(600)` để gán vận tốc.

### 2.3. Cải Thiện Kỹ Năng & Ngăn Chặn Ngắt Chiêu (Skill Interruption)
- **Files Modified:** `Player.cpp`, `PlayerSkillState.h`, `PlayerSkillState.cpp`
- **Thay đổi:**
  - Bổ sung phím kích hoạt Đỡ Đòn (`Block` - Q) và Cầu Lửa (`LongAttack` - U/M) vào `World01State.cpp` do bị thiếu.
  - **Ngăn ngắt chiêu (Cancel Skill):** Cập nhật `Player::useSkill`, từ chối nhận thêm tín hiệu skill mới nếu `currentState` đang là `skillState`. Animation phải chạy xong 100% mới được xài chiêu khác (hoặc lướt).
  - **Ép khựng di chuyển:** Trong `PlayerSkillState::onEnter`, thêm lệnh triệt tiêu vận tốc ngang (`stopLeftRun`, `stopRightRun`). Khi đang chạy đà mà xuất chiêu, nhân vật sẽ phanh đứng lại thay vì trượt dài trên mặt đất.
  - **Giữ chiêu Đỡ Đòn liên tục:** Thêm hàm `resetTimer()` vào `PlayerSkillState`. Riêng với kỹ năng `"Block"`, nếu người chơi giữ phím, timer sẽ liên tục được reset, giúp nhân vật giữ thế thủ vô hạn thay vì bị giật frame mỗi 0.1 giây.

### 2.4. Khắc Phục Lỗi Stun-Lock (Bị Kẹt Khi Nhận Sát Thương)
- **Files Modified:** `Player.cpp`, `CharacterStats.h`
- **Thay đổi:**
  - Phát hiện lỗi nghiêm trọng khi `CombatSystem` tính toán sát thương mỗi frame: Nếu nguồn sát thương (Fireball, Quái) không biến mất, ngay khi trạng thái `hurtState` (0.6s) kết thúc, nhân vật lập tức bị ăn damage và rơi vào `hurtState` lần nữa, dẫn đến việc bị kẹt cứng (không thể di chuyển).
  - **Giải pháp - Cơ chế I-Frames (Thời gian vô địch):**
    - Khai báo thêm `float iframeTimer = 0.0f;` trong `CharacterRuntimeStats`.
    - Khi nhận sát thương trong `Player::takeDamage`, cấp cho người chơi **1.0 giây vô địch**. Do animation hurt chỉ tốn 0.6 giây, người chơi có **0.4 giây** hoàn toàn tự do để lướt hoặc nhảy đi.
    - Cập nhật logic trừ dần `iframeTimer` trong `Player::update`.
    - Thêm một lực đẩy nhẹ lên trên (`knockback y = -200`) khi nhận sát thương để tránh bị dính dưới mặt đất.
    - Đổi `requestState` thành `changeState` trong `takeDamage` để ép nhân vật phản ứng với sát thương dù đang ở trạng thái khóa (như Crouch).

---

## 3. Kết Luận
Việc hợp nhất giữa kiến trúc Map/Camera mới của `binh01` và logic Combat hiện tại đã hoàn tất. Các thay đổi hoàn toàn tuân thủ OOP, sử dụng State Pattern chặt chẽ (giới hạn quyền chuyển State bằng `canExit()`), và tận dụng Command Pattern hiệu quả mà không phá vỡ logic cũ. Code đã sẵn sàng và an toàn để mở rộng trong các Phase tiếp theo.
