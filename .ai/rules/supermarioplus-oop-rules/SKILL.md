---
name: supermarioplus-oop-rules
description: Kỹ năng hướng dẫn Agent tuân thủ nghiêm ngặt các quy tắc lập trình hướng đối tượng (OOP) và quản lý cấu trúc cho dự án SuperMarioPlus.
---

# Kỹ năng: SuperMarioPlus OOP Rules

Kỹ năng này đảm bảo rằng bạn (Agent) khi làm việc trên dự án `SuperMarioPlus` sẽ luôn tuân theo các quy tắc khắt khe về Object-Oriented Programming (OOP), đồng thời đảm bảo sự minh bạch trong việc hướng dẫn người dùng và cập nhật cấu trúc dự án.

## Hướng dẫn từng bước

Bất cứ khi nào bạn được yêu cầu viết code, đề xuất cấu trúc, hoặc refactor dự án SuperMarioPlus, hãy làm theo các nguyên tắc sau:

1. **Tuân thủ nguyên tắc OOP (1 Class / 1 File):**
   - Mọi quyết định kiến trúc phải dựa trên đặc điểm của OOP (Đóng gói, Kế thừa, Đa hình, Trừu tượng).
   - Mỗi file chỉ chứa MỘT class duy nhất (ví dụ: `Player.h` và `Player.cpp`). Không để nhiều class hoặc các struct lớn chung trong một file.

2. **Giải thích chi tiết (Sư phạm):**
   - Khi đề xuất cấu trúc, cơ chế build hoặc ý tưởng thực thi, bạn PHẢI giải thích rõ ý tưởng.
   - Trình bày chức năng của từng hàm, từng class được tạo ra.
   - Giải thích mục đích đằng sau các quyết định đó để người dùng có thể học hỏi cách ứng dụng OOP vào dự án.

3. **Luôn cập nhật `structure.md`:**
   - Khi tạo file mới hoặc thêm class mới, PHẢI ghi vào file `structure.md` cấu trúc dạng cây về mối liên hệ giữa các file/class.
   - Trong `structure.md`, bắt buộc có mục chi tiết: Tác dụng của file/class là gì? Nó được dùng cho file/class nào? Cần thêm file/class nào để bổ trợ cho nó?
   - Cung cấp đủ thông tin tóm tắt trong `structure.md` để các Agent sau này khi đọc sẽ hiểu ngay mục đích và tiến độ dự án mà không tốn token đọc lại toàn bộ code.

4. **Xử lý lệnh `/update structure.md`:**
   - Nếu người dùng gõ lệnh `/update structure.md`, bạn PHẢI tự động duyệt toàn bộ dự án, tìm các file đã sửa/thêm mới và cập nhật chính xác lại vào file `structure.md`.

5. **Quy tắc khi xoá code cũ:**
   - Khi thực thi ý tưởng mà phải xóa nhiều code có sẵn, PHẢI hỏi ý kiến người dùng và phân tích tại sao lại cần xóa để thay code mới.
   - Hạn chế tối đa việc xóa nhiều code cũ, trừ khi người dùng ra lệnh thay thế code cũ bằng ý tưởng mới.
