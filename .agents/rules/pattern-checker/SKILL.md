---
name: pattern-checker
description: Quét và kiểm tra code của dự án xem có tuân thủ Design Pattern và OOP hay không. Kích hoạt khi người dùng gõ lệnh /check pattern.
---

# Kỹ năng: Pattern Checker

Kỹ năng này được sử dụng để phân tích mã nguồn của người dùng nhằm phát hiện các vi phạm về nguyên lý thiết kế (Design Patterns) và Lập trình Hướng đối tượng (OOP).

## Trigger
Kích hoạt tự động khi người dùng gõ dòng lệnh: `/check pattern`

## Hướng dẫn thực thi

Khi được kích hoạt, Agent phải thực hiện các bước sau:

1. **Thu thập thông tin**: Yêu cầu người dùng chỉ định file muốn kiểm tra hoặc tự động phân tích các file code mới được thay đổi gần nhất.
2. **Kiểm tra vi phạm State Pattern**: 
   - Tuyệt đối không được có các khối lệnh `if/else` hoặc `switch/case` lồng nhau để kiểm tra trạng thái (`GameState`, `PlayerState`...) bên trong vòng lặp chính.
   - Mọi trạng thái phải được triển khai thông qua tính đa hình (Polymorphism) và Abstract Base Class (Interfaces).
3. **Kiểm tra vi phạm OOP**:
   - Mỗi file chỉ chứa một Class duy nhất.
   - Không được gộp các hàm xử lý logic (Update) và hàm vẽ (Draw) của nhiều đối tượng vào chung một class không liên quan (ví dụ: `SuperMarioPlus.cpp` chỉ được làm GameManager).
4. **Báo cáo**: 
   - Nếu phát hiện vi phạm: Trích dẫn đoạn code vi phạm, giải thích lý do tại sao sai nguyên lý, và viết lại đoạn code đó theo chuẩn Design Pattern.
   - Nếu code sạch và tuân thủ: Đưa ra lời khen ngợi người dùng.
