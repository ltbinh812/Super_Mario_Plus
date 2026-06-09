---
name: structure-manager
description: Kỹ năng tự động cập nhật file structure.md mỗi khi có file/class mới được tạo.
---

# Kỹ năng: Quản lý file structure.md

Kỹ năng này đảm bảo hệ thống tài liệu kiến trúc của dự án (`structure.md`) luôn được cập nhật theo thời gian thực mỗi khi có sự thay đổi về cấu trúc thư mục hoặc class.

## Hướng dẫn từng bước

Mỗi khi bạn (Agent) tạo file mới hoặc thêm một class mới vào dự án, hãy làm theo các bước sau:

1. **Cập nhật sơ đồ:** Mở file `structure.md` và vẽ lại/bổ sung vào sơ đồ cây mối liên hệ giữa các file/class.
2. **Bổ sung chi tiết:** Thêm một mục chi tiết mô tả về file/class mới:
   - Tác dụng của file/class đó là gì?
   - File/class đó phục vụ cho file/class nào khác?
   - Cần thêm file/class nào để bổ trợ cho nó không?
3. **Lưu trữ bối cảnh:** Ghi chú lại thông tin tổng quan cần thiết để các Agent ở phiên sau khi đọc file này có thể lập tức hiểu tiến độ dự án mà không cần tốn token phân tích lại toàn bộ codebase.
