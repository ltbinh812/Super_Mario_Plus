Đọc các ảnh trong a.json, chest_normal.json, chest_boss.json, chest_enemy.json, explosion.png
Kích thước mặc định là 1 block (16px trong ldtk và 32px trong game)
Entities::Luckeyblock sẽ có 2 trạng thái Active: block_exclamation.png, block_exclamation_active.png
Entities::Chest_boss: tất cả các ảnh trong file chest_boss.json, mỗi rương có thêm 1 ảnh là khi đã mở thì sẽ mờ hơn ảnh gốc
Entities::Chest_normal: tất cả các ảnh trong file chest_normal.json, mỗi rương có thêm 1 ảnh là khi đã mở thì sẽ mờ hơn ảnh gốc
Entities::Coin: coin_gold.png
Entities::Boom: có 2 trạng thái chờ nổ là: bomb.png, bomb_active.png. Sau khi nổ tung là: explosion.png
Entities::Key: key_green.png
Entities::Door: có 2 trạng thái đóng mở: door_closed.png, door_open.png (cửa có kích thước cao 5 block và ngang 3 block)
Entities::Flag: có 2 trạng thái cờ active: flag_off.png, flag_off.png 
Entities::Spring_down: spring_down.png
Entities::Spring_up: spring_up.png
Entities::Spring_left: spring_left.png
Entities::Spring_right: spring_right.png

Entities::Buff: sẽ có 7 loại buff khác nhau trong buff.json, cần 1 class buff để lấy ngẫu nhiên là 1 loại trong 7 loại





Yêu cầu: chuẩn OOP và dùng các design pattern thích hợp, chúng là các item có tương tác vật lý nhưng không di chuyển hay gây sát thương (ngoại trừ Coin, Boom, key, flag, Buff). Lên plan thiết kế các cấu trúc này, để đọc từ các Entities của file world01 02 03 04 05 06.ldtk nối với các file .json và gán asset cho chúng. Và đặt các item này trên các map ở các vị trí đã cho trước trong file .ldtk