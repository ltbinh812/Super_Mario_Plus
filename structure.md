# SuperMarioPlus Project Structure & Progress

**Project Purpose:** A 2D platformer game (Super Mario style) built using C++ and Raylib.

## Tình trạng & Tiến độ dự án hiện tại (Current Status & Progress)
- **Kiến trúc:** Dự án đã được refactor toàn diện theo nguyên tắc OOP (1 file/class) và phân chia thành các module rõ ràng (`abilities`, `characters`, `core`, `environment`, `states`, `ui`).
- **Design Patterns:** Đã áp dụng **State Pattern** cho quản lý vòng lặp game (`GameState`, `CharacterState`) và **Strategy Pattern** cho hệ thống kỹ năng (`AbilityStrategy`), cùng với **Factory Pattern** cho khởi tạo nhân vật (`CharacterFactory`).
- **Hoàn thiện:** Hệ thống quản lý màn hình (Menu, World), vẽ map (Platform, Cloud, Decoration, Background), quản lý nhân vật (Mario, Luigi, Peach, Toad, Wario, Goomba) và thay đổi kỹ năng cơ bản (Dash, Fireball, Float, GroundPound, HighJump) đã hoàn thiện cấu trúc. Các file đã được dọn dẹp lỗi hiển thị tiếng Việt. Menu settings và UI chọn nhân vật cơ bản đã có mặt.
- **Đang dở dang (WIP):** Logic chi tiết của từng state nhân vật (chạy, nhảy, va chạm) và các tính năng Save/Load trong UI Settings vẫn đang trong quá trình phát triển (WIP).

## File & Class Relationships

```text
SuperMarioPlus/
├── assets/
├── build/
├── build_mingw/
├── include/
│   ├── abilities/
│   │   ├── AbilityStrategy.h
│   │   ├── DashAbility.h
│   │   ├── FireballAbility.h
│   │   ├── FloatAbility.h
│   │   ├── GroundPoundAbility.h
│   │   └── HighJumpAbility.h
│   ├── characters/
│   │   ├── Character.h
│   │   ├── CharacterFactory.h
│   │   ├── Goomba.h
│   │   ├── Luigi.h
│   │   ├── Mario.h
│   │   ├── Peach.h
│   │   ├── Player.h
│   │   ├── Toad.h
│   │   └── Wario.h
│   ├── core/
│   │   ├── CameraManager.h
│   │   ├── Entity.h
│   │   ├── GameManager.h
│   │   └── SuperMarioPlus.h
│   ├── environment/
│   │   ├── Background.h
│   │   ├── Cloud.h
│   │   ├── Decoration.h
│   │   └── Platform.h
│   ├── states/
│   │   ├── CharacterState.h
│   │   ├── GameState.h
│   │   ├── Menu.h
│   │   ├── PlayerStates.h
│   │   └── World.h
│   └── ui/
│       ├── CharacterSelectionOverlay.h
│       ├── HUD.h
│       ├── OverlayUI.h
│       └── SettingsOverlay.h
├── src/
│   ├── abilities/
│   ├── characters/
│   │   ├── Character.cpp
│   │   ├── CharacterFactory.cpp
│   │   ├── Goomba.cpp
│   │   ├── Luigi.cpp
│   │   ├── Mario.cpp
│   │   ├── Peach.cpp
│   │   ├── Player.cpp
│   │   ├── Toad.cpp
│   │   └── Wario.cpp
│   ├── core/
│   │   ├── CameraManager.cpp
│   │   ├── GameManager.cpp
│   │   ├── main.cpp
│   │   └── SuperMarioPlus.cpp
│   ├── environment/
│   │   ├── Background.cpp
│   │   ├── Cloud.cpp
│   │   ├── Decoration.cpp
│   │   └── Platform.cpp
│   ├── states/
│   │   ├── Menu.cpp
│   │   ├── PlayerStates.cpp
│   │   └── World.cpp
│   └── ui/
│       ├── CharacterSelectionOverlay.cpp
│       ├── HUD.cpp
│       └── SettingsOverlay.cpp
├── third_party/
│   └── raylib/
├── workflow/
│   ├── PLAN.md
│   ├── README.md
│   ├── application.puml
│   ├── entity.puml
│   ├── infrastructure.puml
│   ├── system.puml
│   └── world.puml
├── .cursorrules
├── .gitignore
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
└── structure.md
```

### Detailed File / Class Information

#### Core
- **`main.cpp`**: Entry point. Mở cửa sổ, thiết lập Raylib và chạy vòng lặp chính.
- **`SuperMarioPlus`**: Cung cấp `RunGame()`, thiết lập GameManager.
- **`GameManager`**: Trái tim của State Pattern, quản lý state hiện tại của game (Menu, World).
- **`CameraManager`**: Theo dõi Player, tính toán offset để vẽ màn hình cuộn.
- **`Entity`**: Base class cho mọi vật thể trong game (tọa độ, tốc độ).

#### States
- **`GameState`**: Abstract class quy định các hàm `HandleInput()`, `Update()`, `Draw()` cho màn chơi.
- **`MenuState` (`Menu`)**: Trạng thái màn hình bắt đầu game.
- **`World1_1State` (`World`)**: Quản lý màn hình chơi chính (Mario, Quái, Môi trường).
- **`CharacterState` / `PlayerStates`**: Base interface và các lớp triển khai cho State Pattern của nhân vật (Idle, Running, Jumping).

#### Characters
- **`Character` / `Player`**: Base class đại diện cho nhân vật. Player kế thừa Character, chứa input và xử lý logic riêng.
- **`CharacterFactory`**: Factory Pattern để tự động sinh ra các nhân vật dựa trên chuỗi định danh (Tên).
- **`Mario, Luigi, Peach, Toad, Wario`**: Các lớp cụ thể đại diện cho người chơi, được override các kỹ năng tương ứng.
- **`Goomba`**: Kẻ thù cơ bản, kế thừa từ `Character`.

#### Abilities (Strategy Pattern)
- **`AbilityStrategy`**: Interface định nghĩa `Execute()`.
- **`DashAbility`, `FireballAbility`, `FloatAbility`, `GroundPoundAbility`, `HighJumpAbility`**: Các lớp triển khai kỹ năng cụ thể cho từng nhân vật. Gắn linh hoạt vào Character.

#### Environment
- **`Background`, `Cloud`, `Decoration`, `Platform`**: Các lớp thực thể quản lý việc vẽ hình nền, nền tảng vật lý (có xử lý va chạm), mây trôi và các phụ kiện trang trí trong World.

#### UI
- **`HUD`**: Lớp chứa logic in thông tin lên màn hình (Máu, Điểm, Thời gian).
- **`OverlayUI`**: Interface cho giao diện hiển thị đè lên màn chơi (Settings, Chọn nhân vật).
- **`SettingsOverlay`**: Menu tùy chỉnh (Save, Load, Về Menu chính).
- **`CharacterSelectionOverlay`**: UI cho phép 2 người chơi swap chọn nhân vật (Mario, Luigi, v.v.).

#### Root & Misc Folders
- **`assets/`**: Chứa các tài nguyên của game (âm thanh, hình ảnh, font chữ...). Hiện tại đang trống.
- **`build/`, `build_mingw/`**: Thư mục chứa các file cấu hình và executable sinh ra từ quá trình build (CMake).
- **`third_party/`**: Chứa các thư viện bên thứ 3, cụ thể là `raylib`.
- **`workflow/`**: Chứa các file tài liệu thiết kế hệ thống (UML diagrams) và kế hoạch dự án (`PLAN.md`).
- **`CMakeLists.txt`, `CMakePresets.json`**: Cấu hình build dự án sử dụng CMake.
- **`.cursorrules`**: Các luật cấu hình cho AI / Cursor.

---
*Note: History of monolithic structure has been pruned to keep documentation concise as per rules.*
