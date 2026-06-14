---
name: structure-updater
description: Kỹ năng quét toàn bộ dự án và cập nhật lại structure.md khi người dùng gõ lệnh /update structure.md.
---

# Kỹ năng: Cập nhật structure.md

Kỹ năng này chịu trách nhiệm đồng bộ hoá lại file `structure.md` với thực trạng code hiện tại của dự án khi nhận được lệnh từ người dùng.

## Hướng dẫn từng bước

Khi người dùng gõ lệnh `/update structure.md`, hãy làm theo các bước sau:

1. **Quét dự án:** Đọc qua toàn bộ cấu trúc thư mục (đặc biệt là `src` và `include`) để tìm các file đã sửa hoặc thêm mới.
2. **Phân tích:** Nhận diện các class/file mới hoặc những liên kết bị thay đổi so với tài liệu hiện tại trong `structure.md`.
3. **Đồng bộ cấu trúc:** Cập nhật lại sơ đồ cây và giải thích chức năng chi tiết cho tất cả các file/class. Phải đảm bảo mô tả cấu trúc đầy đủ nhất có thể.
4. **Báo cáo tiến trình (CỰC KỲ QUAN TRỌNG):** Phải tổng hợp và ghi rõ một mục về "Tình trạng & Tiến độ dự án hiện tại". Mục này phải tóm tắt đầy đủ dự án đang làm tới đâu, logic nào đã hoàn thiện, phần nào đang dở dang. Mục tiêu cốt lõi là: **Bất kỳ Agent nào ở lần sau khi mở project, chỉ cần đọc file `structure.md` này là sẽ hiểu ngay lập tức 100% bối cảnh và tiến độ dự án mà không cần phải duyệt lại code.**
5. **Dọn dẹp lịch sử (CẮT TỈA NỘI DUNG):** Tự động xóa bỏ những thông tin tiến trình cũ, những lịch sử cập nhật không còn giá trị tham khảo hoặc đã lỗi thời. Chỉ giữ lại thực trạng hiện tại để tránh file quá dài, giúp Agent đọc nhanh và không bị nhiễu thông tin.
