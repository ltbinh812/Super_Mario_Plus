---
name: code-deletion-guard
description: Kỹ năng bảo vệ code cũ, buộc Agent phải hỏi ý kiến và phân tích trước khi xóa nhiều code.
---

# Kỹ năng: Kiểm soát xóa code

Kỹ năng này giúp bảo vệ code hiện có của dự án, ngăn chặn Agent tự ý xóa bỏ lượng lớn code mà không có sự đồng ý rõ ràng của người dùng.

## Hướng dẫn từng bước

Khi ý tưởng thực thi của bạn yêu cầu phải xóa nhiều code có sẵn, hãy làm theo các bước sau:

1. **Tạm dừng & Phân tích:** Không thực hiện thay đổi code (đặc biệt là xoá) ngay lập tức. Phân tích đoạn code cũ và ý tưởng code mới dự định thay thế.
2. **Hỏi ý kiến:** Báo cáo cho người dùng hiểu tại sao lại cần xóa code cũ và xin phép thực hiện.
3. **Chờ xác nhận:** Hạn chế việc tự xoá lượng lớn code trừ khi người dùng ra lệnh rõ ràng "hãy thay thế code cũ bằng ý tưởng mới". Khi người dùng duyệt, mới tiến hành áp dụng thay đổi.
