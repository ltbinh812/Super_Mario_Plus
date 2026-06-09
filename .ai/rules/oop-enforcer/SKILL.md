---
name: oop-enforcer
description: Kỹ năng bắt buộc Agent tuân thủ nguyên tắc OOP (1 Class / 1 File) khi làm việc trên dự án SuperMarioPlus.
---

# Kỹ năng: Tuân thủ OOP

Kỹ năng này đảm bảo Agent luôn tuân thủ chặt chẽ các nguyên tắc Lập trình hướng đối tượng (OOP) khi viết code hoặc cấu trúc lại dự án.

## Hướng dẫn từng bước

Khi người dùng yêu cầu viết code mới hoặc refactor, hãy làm theo các bước sau:

1. **Kiểm tra nguyên tắc:** Đảm bảo mọi quyết định kiến trúc đều dựa trên 4 đặc điểm của OOP (Đóng gói, Kế thừa, Đa hình, Trừu tượng).
2. **Cấu trúc file:** Bắt buộc áp dụng nguyên tắc "1 Class / 1 File". Mỗi class phải nằm trong một cặp file (ví dụ: `Player.h` và `Player.cpp`).
3. **Phân tách:** Không bao giờ gộp nhiều class hoặc các struct có chức năng lớn vào chung một file duy nhất.
4. **Áp dụng Design Pattern:** Khi thiết kế các logic lớn (State, Factory, Singleton), bắt buộc tuân thủ nguyên lý mẫu thiết kế thay vì dùng các lệnh kiểm tra `if/else` dài dòng.
