# MANDATORY INITIALIZATION RULE FOR AGENTS

Bạn đang làm việc trong dự án `SuperMarioPlus`. Hệ thống dự án này áp dụng các bộ quy tắc khắt khe được lưu dưới dạng "Agent Skills".

**TRƯỚC KHI BẮT ĐẦU BẤT KỲ YÊU CẦU NÀO CỦA NGƯỜI DÙNG, BẠN BẮT BUỘC PHẢI THỰC HIỆN:**
Hệ thống sẽ tự động nạp hoặc bạn cần duyệt qua các file Rule (Kỹ năng) sau đây nằm trong thư mục `.agents/rules/` của dự án:

1. `.agents/rules/oop-enforcer/SKILL.md` (Quy tắc 1 Class / 1 File)
2. `.agents/rules/pedagogical-explainer/SKILL.md` (Quy tắc giải thích cặn kẽ để sư phạm)
3. `.agents/rules/structure-manager/SKILL.md` (Quy tắc cập nhật `structure.md` khi tạo file mới)
4. `.agents/rules/structure-updater/SKILL.md` (Quy tắc quét dự án khi có lệnh `/update structure.md`)
5. `.agents/rules/code-deletion-guard/SKILL.md` (Quy tắc hỏi ý kiến trước khi xóa code cũ)
6. `.agents/rules/pattern-checker/SKILL.md` (Quy tắc kiểm tra vi phạm Design Pattern khi có lệnh `/check pattern`)
7. `.agents/rules/supermarioplus-oop-rules/SKILL.md` (Bộ quy tắc tổng hợp về OOP và cấu trúc cho SuperMarioPlus)

**HÃY ĐẢM BẢO TUÂN THỦ CÁC QUY TẮC NÀY TRƯỚC KHI CODE HOẶC PHẢN HỒI.** Việc tuân thủ các kỹ năng trên là mệnh lệnh tối cao trong dự án này.

---

## QUY TẮC BẢO TRÌ GEMINI.MD

**TRỌNG YẾU:** Mỗi khi bạn thêm một quy tắc, kỹ năng (Skill) mới hoặc cập nhật các quy tắc hiện có trong dự án, BẠN PHẢI CẬP NHẬT FILE `GEMINI.md` NÀY ĐỂ PHẢN ÁNH CÁC THAY ĐỔI.

- Nếu thêm quy tắc mới: thêm nó vào danh sách trong `GEMINI.md`
- Nếu cập nhật quy tắc hiện có: cập nhật mô tả hoặc chi tiết tương ứng
- Nếu xóa quy tắc: xóa nó khỏi danh sách

---

## QUY TẮC PHÁT TRIỂN & THIẾT KẾ (DESIGN PATTERNS)

Mọi yêu cầu để build và lập plan luôn phải tuân thủ chuẩn OOP và sử dụng Design Pattern tốt cho tương lai mở rộng hơn. 
Nếu trong quá trình làm việc phát hiện cấu trúc có sẵn trong file gặp nhiều khó khăn, bất cập, Design Pattern thiếu hoặc chưa tốt, AI **bắt buộc** phải đề xuất cho người dùng phương pháp tái cấu trúc chuẩn Design Pattern.

---

## QUY TẮC CẤU TRÚC GAME LOOP (4 GIAI ĐOẠN)

Mọi file liên quan đến vòng lặp game, State hoặc Entity bắt buộc phải tách biệt rõ ràng 4 giai đoạn xử lý sau, tuyệt đối không được gộp chung logic của chúng:
1. `handleInput()`: Chỉ đọc và ghi nhận thao tác (phím/chuột) của người chơi.
2. `process()`: Tiền xử lý, tính toán logic (ví dụ: AI quyết định hành động, hoặc kiểm tra điều kiện kích hoạt).
3. `update(float dt)`: Chỉ cập nhật trạng thái, vị trí, vật lý, bộ đếm thời gian dựa trên kết quả của input và process.
4. `render(float alpha)`: Chỉ chứa code vẽ hình ảnh lên màn hình, tuyệt đối không thay đổi trạng thái (state) hay tọa độ vật lý tại đây.
