The submission must include the below documents:

- AI Usage Declaration (Markdown + PDF)

- The report (Markdown + PDF) including class diagram(s), applied design patterns and the reasoning of your design

- List of features of the project, 0.25pts / 10pts for each feature.

- The source code

- Member contribution in the project: member_contribution.xlsx



~~~~~~~~~~~~~~~~~~~~~~~~~~~
Yêu cầu sửa lại:

những tính năng này phải đưa lên đầu

PlayerInputsMovementCollision
EnemyBehavior
PowerUpsItems
3 LevelCompletion
Sounds
OOD
DesignPatterns
AI
MultiplePlayers


xem các mục dưới đây, có bổ sung thêm tính năng nào lớn, đủ, không quá nhỏ cho feature_list.md không


multiple player
AI
map building
PlayerInputsMovementCollision
EnemyBehavior
PowerUpsItems
12 LevelCompletion
Sounds
OOD
DesignPatterns
MultiplePlayers



bonus:
save game
shop
Ngoài tương tác giữa player - enemy, còn tương tác giữa các block đặc biệt trên map với entities (lava/poison gây sát thương trực tiếp lên cả player, enemy - nước làm lở lửng, di chuyển chậm, có thanh thở)
combat system (các frame cho 1 skill, kết hợp nút)
nhiều loại boss/quái
nhiều nhân vật khác nhau để lựa chọn
chế độ cốt chuyện 1 người và chế độ pvp 2 người và chế độ custom map (1 hoặc 2 player tùy ý)
Hệ thống mana, nhiều skill khác nhau
settings
 



















1. Hệ thống Quản trị Menu & Cấu hình (Menu & Settings System)
Phân luồng màn hình Menu (Scene Management):
Chuyển đổi trạng thái giữa Menu chính, Chọn nhân vật/chế độ, Màn chơi tùy chỉnh và Cài đặt.
Hệ thống Cài đặt Phím điều khiển (Key Mapping System):
Hỗ trợ gán lại phím độc lập cho cả Player 1 và Player 2 (Input Rebinding).
Lưu trữ cấu hình phím tùy biến của người chơi.
Hệ thống Bộ trộn Âm thanh (Audio Mixer Settings):
Quản lý thanh trượt điều chỉnh âm lượng riêng biệt cho từng kênh: Master Volume, BGM (Nhạc nền), Player SFX (Hiệu ứng người chơi), và Enemy SFX (Hiệu ứng quái vật).
2. Hệ thống Chế độ chơi & Trình chỉnh sửa Màn (Game Modes & Level Editor)
Chế độ Chơi đơn (Single-player Mode): Trải nghiệm cốt truyện (Story Mode) theo tiến trình màn chơi.
Chế độ Hai người chơi (Two-player Mode):
PvP (Đối kháng nội bộ): Hai người chơi sử dụng chung màn hình hoặc thi đấu trực tiếp.
Co-op Story: Hai người chơi phối hợp cùng vượt qua ải cốt truyện.
Trình chỉnh sửa Bản đồ (Custom Map Editor):
Hệ thống Palette gạch (Block Palette): Chứa danh mục các loại block môi trường khác nhau.
Thiết lập vị trí khởi đầu (Spawn Point Setter): Đặt tọa độ xuất phát cho nhân vật.
Cơ chế Thêm/Xóa Block (Grid-based Placement & Eraser): Cho phép đặt và xóa các ô gạch trực tiếp trên hệ tọa độ bản đồ.
3. Hệ thống Nhân vật & Cơ chế Điều khiển (Player Controller System)
Cơ sở dữ liệu Nhân vật (Character Roster):
Hỗ trợ 6 nhân vật có thể điều khiển, mỗi nhân vật sở hữu bộ Sprite/Animation riêng biệt.
Máy trạng thái di chuyển (Movement State Machine):
Cơ bản: Run (Chạy), Jump (Nhảy), Crouch (Ngồi), Dash (Lướt nhanh né đòn).
Phòng thủ: Block (Chặn đòn - giảm hoặc triệt tiêu sát thương).
Địa hình đặc biệt: Climb (Leo trèo trên các block đặc biệt), Swim (Bơi khi ở trong vùng nước).
Quản lý Chỉ số Sinh tồn (Vital & Resource Management):
Thanh Máu (Health Bar): Xử lý nhận sát thương (Hurt), bất tử tạm thời (I-frames), và xử lý chết (Die).
Thanh Năng lượng (Mana Bar): Tiêu hao năng lượng khi xuất chiêu đặc biệt hoặc bắn phép.
Thanh Dưỡng khí (Oxygen Bar): Kích hoạt khi bơi dưới nước, tự động giảm dần theo thời gian và trừ máu khi hết dưỡng khí.
4. Hệ thống Kỹ năng & Cơ chế Chiến đấu (Combat & Skill Mechanics)
Thao tác / Kỹ năng
Cơ chế Kỹ thuật (Technical Implementation)
Combo 4 đòn đánh
Hệ thống Input Buffer & Timing Window: Đòn đánh kế tiếp chỉ kích hoạt nếu người chơi nhấn đúng khung thời gian (Timing).
Đòn kết thúc (Attack 4)
Kích hoạt hiệu ứng nổ diện rộng (AoE Explosion) và gây sát thương lan.
Jump Attack
Xử lý đòn đánh trên không khi đang trong trạng thái Jump/Fall.
Crouch Attack
Đòn đánh tầm thấp khi đang trong trạng thái Crouch.
Fireball Attack
Sinh ra một thực thể đạn (Projectile) bay theo hướng chỉ định, xử lý va chạm và nổ.
Special Skill (Chiêu cuối)
Kỹ năng đặc biệt yêu cầu đủ thanh Mana, có hoạt ảnh và hiệu ứng riêng.
Phản hồi Va chạm (Hitbox/Hurtbox)
Hiệu ứng Knockback (đẩy lùi), rung màn hình, và hiệu ứng nổ tương ứng với từng đòn.

5. Hệ thống Vật phẩm, Kinh tế & Hiệu ứng Buff (Item & Status Effect System)
Hệ thống Kinh tế & Thu thập (Economy & Collectibles):
Coins: Tiền tệ thu thập từ quái vật hoặc rải rác trên màn chơi.
Keys: Chìa khóa dùng để mở khóa Cửa (Door) hoặc Cổng qua màn (Gate).
Vật phẩm Tương tác Môi trường (Interactive Level Objects):
Lucky Block: Khối ngẫu nhiên khi đập vỡ sẽ rơi ra vật phẩm hoặc hiệu ứng.
Explosive Bomb: Thùng bom/thuốc nổ gây sát thương xung quanh khi bị kích hoạt.
Springs: Đệm lò xo tạo lực nảy lớn cho nhân vật.
Chest: Rương kho báu mở ra tiền/bình thuốc.
Shop: NPC/Giao diện cửa hàng dùng Coin để trao đổi đồ.
Finish Gate: Cổng kết thúc màn chơi khi thỏa mãn điều kiện (ví dụ: có đủ chìa khóa).
Hệ thống Hiệu ứng Bình thuốc (Potions & Status Effects Manager):
Loại Potion
Hiệu ứng Kỹ thuật
Heal
Hồi lại một lượng HP nhất định.
Strength
Tăng hệ số sát thương đầu ra (Attack Multiplier) trong thời gian giới hạn.
Speed Boost
Tăng tốc độ chạy (Movement Speed) trong thời gian giới hạn.
Jump Boost
Tăng lực nhảy (Jump Force) trong thời gian giới hạn.
Shield / Invincible
Tạo lớp khiên chặn sát thương / Bật trạng thái bất tử (God Mode tạm thời).
Magnet
Tạo vùng từ trường tự động hút Coin và Item xung quanh về phía người chơi.
Time Stop
Đóng băng hoặc làm chậm toàn bộ thời gian của AI và đạn đạo của quái.
Poison
Gây sát thương theo thời gian (Damage over Time - DoT).
Random
Chọn ngẫu nhiên một trong các hiệu ứng trên khi nhặt.

6. Hệ thống Trí tuệ Nhân tạo Kẻ địch & Trùm (Enemy & Boss AI System)
       ┌──────────────┐
        │     IDLE     │◄────────┐
        └──────┬───────┘         │
               │ (Player unseen) │
        ┌──────▼───────┐         │ (Player lost)
        │    PATROL    │─────────┤
        └──────┬───────┘         │
               │ (Player detected)
        ┌──────▼───────┐
        │ CHASE / RUN  │
        └──────┬───────┘
               │ (In attack range)
        ┌──────▼───────┐
        │    ATTACK    │ (Combos / Special / Projectile)
        └──────────────┘

9 Loại Quái thường (Mob AI):
Finite State Machine (FSM): Các trạng thái Idle, Patrol (Tuần tra), Chase (Đuổi theo), Attack, Hurt, Die.
Cảm biến Không gian (Sensory & Pathfinding):
Vision Zone: Nhận diện người chơi khi bước vào vùng quan sát.
Cliff Detection: Cảm biến Raycast phát hiện mép vực để tự động quay đầu tránh rơi.
Obstacle Handling: Tự động nhảy khi gặp vật cản.
Teleport Ability: Dịch chuyển tức thời đến vị trí mục tiêu.
Loot Drop: Tự động sinh ra Coin khi chết.
6 Loại Trùm (Boss AI):
Kế thừa toàn bộ hành vi di chuyển và cảm biến của quái thường.
Mẫu Tấn công Nâng cao (Advanced Attack Patterns):
Combo đánh thường 1, 2, 3, 4.
Đòn đánh nổ diện rộng (Explosion Attack).
Đòn đánh tầm xa (Long-range Projectile Attack).
Kỹ năng tối thượng / Đặc biệt (Boss Ultimate Skill).
Loot Drop: Sinh ra Key để người chơi mở cổng qua màn tiếp theo.
7. Hệ thống Cốt truyện, Điện ảnh & Camera (Story & Camera Controller)
Dialogue & Cutscene Controller:
Hệ thống hội thoại hiển thị chữ từng ký tự, kèm ảnh đại diện nhân vật.
Tạm khóa điều khiển nhân vật trong lúc diễn hoạt sự kiện.
Cinematic Camera Controller:
Target Tracking: Camera theo dõi mượt mà (Smooth Follow/Lerp) theo vị trí người chơi.
Cinematic Pan & Zoom: Tự động lia máy (Pan) hoặc phóng to/thu nhỏ (Zoom) tiêu cự vào các điểm nhấn cốt truyện, phòng Boss hoặc bẫy kích hoạt.
8. Hệ thống Lưu trữ Dữ liệu (Save/Load & Checkpoint System)
Checkpoint Tracker: Kích hoạt cờ (Flag) khi người chơi chạm vào, cập nhật tọa độ hồi sinh mới nhất.
Data Serialization:
Lưu trữ các biến trạng thái: Tổng số Coin, trạng thái các Buff/Effect đang kích hoạt, trạng thái màn chơi và vị trí hồi sinh.
9. Hệ thống Quản lý Âm thanh (Audio Engine & Event Triggers)
Background Music Manager:
Nhạc nền riêng cho Menu chính và nhạc nền đặc thù cho từng bản đồ chơi.
Sound Effects & Animation Sync:
Kích hoạt âm thanh chiêu thức, âm thanh trúng đòn, hiệu ứng nổ.
Keyframe-based Footsteps: Gắn âm thanh bước chân trực tiếp vào từng khung hình chuyển động (Animation Keyframe) để âm thanh khớp chính xác với bước chạy của nhân vật.


