# Kế Hoạch Bổ Sung Các Loại Block (Chi Tiết Theo Từng World)

Sau khi dùng script phân tích kỹ dữ liệu của từng map (`world01` đến `world06`), dưới đây là danh sách toàn bộ các Block đã được sử dụng, phân loại rõ ràng theo **World**, **Loại Vật Lý (Physics)** và **Hình Ảnh (Terrain/Visual)**, kèm theo **Khối Thực Thể (Entity)** của Mario. 

Bạn hãy kiểm tra và báo cho tôi biết bạn muốn biến những khối nào dưới đây thành khối có thể tương tác (ví dụ: đập vỡ, chạm vào có hiệu ứng, v.v.).

---

## 1. Danh Sách Theo Từng World (Terrain & Physics)

> Đây là các tile và thuộc tính va chạm đã được họa sĩ/người thiết kế đặt sẵn vào các màn chơi.

### 🍄 World 1 (map01)
- **Physics (Va chạm):** `Solid` (cứng), `OneWay` (một chiều), `Ladder` (thang), `Water` (nước), `Hazard` (nguy hiểm).
- **Terrain (Tile):** `dirt` (đất), `stone` (đá), `bridge` (cầu), `water` (nước), `hoa_sen`, `hoa_sen1`, `hoa_sen2` (sen), `hide`, `hide_dirt` (đất ẩn).

### 🏜️ World 2 (map02)
- **Physics:** `Solid`, `OneWay`, `Ladder`, `Water`, `Hazard`.
- **Terrain:** `walls` (tường).

### ❄️ World 3 (map03)
- **Physics:** `Solid`, `OneWay`, `Ladder`, `Water`, `Hazard`, `Die` (chết ngay).
- **Terrain:** `dirt`, `stone`, `ladder` (thang hiển thị).

### 👻 World 4 (map04)
- **Physics:** `Solid`, `OneWay`, `Ladder`, `Water`, `Hazard`, `Die`.
- **Terrain:** `walls`, `water`.

### ☁️ World 5 (map05) - (Nhiều loại block nhất)
- **Physics:** `Solid`, `OneWay`, `Ladder`, `Water`, `Hazard`, `Die`, `Cloud` (mây), `Lava` (nham thạch), `Poison` (độc), `Slope` (dốc), `Vine` (dây leo).
- **Terrain:** `dirt`, `dirt_cam`, `dirt_fly`, `dirt_grass`, `dirt_xanh`, `cloud`, `grass`, `leaf1`, `leaf2`, `tree`, `wood`, `fence`, `vence`, `lava`, `poison`, `water`, `oneway`.

### 🌋 World 6 (map06)
- **Physics:** `Solid`, `Ladder`, `Water`.
- **Terrain:** `walls`, `water`.

*(Ghi chú: Physics `Solid` đã được hệ thống bắt va chạm cơ bản. Các physics như `Water`, `Ladder`, `Lava` cần lập trình thêm logic bơi/leo/sát thương).*

---

## 2. Nhóm Entity Blocks (Khối Tương Tác Của Mario)

Đây là các khối đặc trưng (thường đặt riêng biệt) người chơi có thể lấy đầu húc hoặc đạp lên:

- [x] **LuckyBlock (Khối dấu hỏi):** Đã hoàn thiện logic.
- [ ] **ExclamationBlock (Khối chấm than):** Đã có data/hình ảnh nhưng chưa code.
- [ ] **BrickBlock (Khối gạch vỡ):** Khối gạch thông thường, Player có thể đập vỡ.
- [ ] **HiddenBlock (Khối tàng hình):** Bình thường không thấy, nhảy lên đội đầu sẽ hiện ra.
- [ ] **NoteBlock (Khối nảy):** Đạp lên hoặc đội lên sẽ văng cao.
- [ ] **DonutBlock (Khối bánh rơi rớt):** Đứng trên lâu quá sẽ bị rớt xuống.
- [ ] **IceBlock (Khối băng):** Bề mặt trơn trượt.

---

## 💡 Câu Hỏi Mở (User Review Required)

> [!IMPORTANT]
> **Dựa trên danh sách đầy đủ trên, bạn muốn tôi lập trình thêm tính năng cho những block nào?**

Một số gợi ý phổ biến bạn có thể chọn:
1. **Làm logic Entity Block:** Code thêm `BrickBlock` (để đập vỡ), `HiddenBlock`, `ExclamationBlock`.
2. **Làm logic Vật lý đặc thù:** Code chức năng leo thang cho `Ladder` ở World 1-6, chức năng bơi cho `Water` (ở World 1,4,6), chức năng trượt/gây sát thương cho `Lava`/`Poison` ở World 5.
3. **Biến Tile thành Entity:** Bạn có muốn khối `stone` ở World 1, 3 bị đập vỡ như gạch không? 

Hãy liệt kê các khối bạn chọn để tôi bắt đầu code!
