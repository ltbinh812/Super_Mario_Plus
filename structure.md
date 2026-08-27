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
│   │   ├── Game.h
│   │   ├── StateManager.h
│   │   └── Command.h
│   ├── cutscene/
│   │   ├── CutsceneManager.h
│   │   ├── CutsceneScript.h
│   │   └── CutsceneTrigger.h
│   ├── dialogue/
│   │   ├── DialogueBox.h
│   │   ├── DialogueData.h
│   │   ├── DialogueLoader.h
│   │   └── DialogueRegistry.h
│   ├── environment/
│   │   ├── Background.h
│   │   ├── CameraFollowMode.h
│   │   ├── CameraPanMode.h
│   │   ├── CameraZoomMode.h
│   │   ├── Cloud.h
│   │   ├── Decoration.h
│   │   ├── ICameraMode.h
│   │   ├── MapCamera.h
│   │   ├── Platform.h
│   │   └── TileMap.h
│   ├── states/
│   │   ├── CharacterState.h
│   │   ├── GameState.h
│   │   ├── IntroState.h
│   │   ├── Menu.h
│   │   ├── PlayerStates.h
│   │   ├── SettingState.h
│   │   └── World.h
│   └── ui/
│       ├── CharacterSelectionOverlay.h
│       ├── HUD.h
│       ├── PlayerHUD.h
│       ├── OverlayUI.h
│       ├── SettingsOverlay.h
│       └── UIComponent.h
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
│   │   ├── Game.cpp
│   │   ├── StateManager.cpp
│   │   └── main.cpp
│   ├── cutscene/
│   │   ├── CutsceneManager.cpp
│   │   └── CutsceneTrigger.cpp
│   ├── dialogue/
│   │   ├── DialogueBox.cpp
│   │   ├── DialogueLoader.cpp
│   │   └── DialogueRegistry.cpp
│   ├── environment/
│   │   ├── Background.cpp
│   │   ├── CameraFollowMode.cpp
│   │   ├── CameraPanMode.cpp
│   │   ├── CameraZoomMode.cpp
│   │   ├── Cloud.cpp
│   │   ├── Decoration.cpp
│   │   ├── MapCamera.cpp
│   │   ├── Platform.cpp
│   │   └── TileMap.cpp
│   ├── states/
│   │   ├── GameState.cpp
│   │   ├── IntroState.cpp
│   │   ├── Menu.cpp
│   │   ├── PlayerStates.cpp
│   │   ├── SettingState.cpp
│   │   └── World.cpp
│   └── ui/
│       ├── CharacterSelectionOverlay.cpp
│       ├── HUD.cpp
│       ├── PlayerHUD.cpp
│       ├── SettingsOverlay.cpp
│       └── UIComponent.cpp
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
- **`Game`**: Lớp chính quản lý vòng lặp game (Game Loop) với cơ chế Fixed-Timestep (tích lũy `accumulator`) giúp logic vật lý chạy ổn định trên mọi FPS.
- **`StateManager`**: Cốt lõi của State Pattern kết hợp Command Pattern (PushCommand), quản lý các state hiện tại bằng stack (Ngăn xếp).
- **`Command`**: Định nghĩa cấu trúc lệnh (Push, Pop, Change, Clear) để chuyển đổi State an toàn mà không bị tight-coupling.
- **`CameraManager`**: Theo dõi Player, tính toán offset để vẽ màn hình cuộn.
- **`Entity`**: Base class cho mọi vật thể trong game (tọa độ, tốc độ).

#### States
- **`GameState`**: Abstract class quy định các hàm `HandleInput()`, `Update(float dt)`, `Render(float alpha) const` cho màn chơi, và hỗ trợ gửi `Command` ngược lên `StateManager`.
- **`IntroState`**: Màn hình giới thiệu ban đầu, có nút chuyển sang Menu hoặc Setting.
- **`MenuState` (`Menu`)**: Màn hình chọn nhân vật và bắt đầu game.
- **`World1_1State` (`World`)**: Quản lý màn hình chơi chính (Mario, Quái, Môi trường). Bắt các OverlayUI khi ấn phím đặc biệt.
- **`SettingState`**: Màn hình thiết lập hệ thống từ Intro.
- **`CharacterState` / `PlayerStates`**: Base interface và các lớp triển khai cho State Pattern của nhân vật (Idle, Running, Jumping).

#### Characters
- **`Character` / `Player`**: Base class đại diện cho nhân vật. Player kế thừa Character, chứa input và xử lý logic riêng.
- **`CharacterFactory`**: Factory Pattern để tự động sinh ra các nhân vật dựa trên chuỗi định danh (Tên).
- **`Mario, Luigi, Peach, Toad, Wario`**: Các lớp cụ thể đại diện cho người chơi, được override các kỹ năng tương ứng.
- **`Goomba`**: Kẻ thù cơ bản, kế thừa từ `Character`.
- **`Boss`**: Kế thừa `Mob`, hỗ trợ cơ chế Cutscene ID (Observer pattern) thông qua hàm `onCutsceneStart` và `onCutsceneEnd` để đồng bộ Boss Intro.
- **Enemy States (`EnemyIdleState`, `EnemyRunState`, `EnemyAttackState`, `EnemyHurtState`, `EnemyDieState`, `EnemySkillState`)**: Các state AI của kẻ địch (Mob), hỗ trợ tự động tìm Player (distance check), tạo hitbox qua `CombatSystem`, xử lý đẩy lùi (knockback), và đặc biệt `EnemySkillState` hỗ trợ random xuất chiêu dựa trên `IEnemySkill`.
- **Boss States (`BossIdleState`, `BossIntroState`)**: Các state chuyên biệt dành cho Boss để xử lý logic chờ Cutscene và chống Animation Popping.

#### Abilities & Skills (Strategy Pattern)
- **`AbilityStrategy`**: Interface định nghĩa `Execute()` cho các kỹ năng của Player.
- **`DashAbility`, `FireballAbility`, `FloatAbility`, `GroundPoundAbility`, `HighJumpAbility`**: Các lớp triển khai kỹ năng cụ thể cho từng nhân vật. Gắn linh hoạt vào Character.
- **`IEnemySkill`**: Interface độc lập (Decoupled) dùng riêng cho AI của Quái/Boss. Cung cấp hàm `execute(Mob&)` cùng với logic tính toán hitbox, damage, thời gian thi triển.
- **`BasicMeleeEnemySkill`**: Kỹ năng mẫu cơ bản dành cho Quái (tự động gây sát thương nếu Player nằm trong tầm đánh).

#### Environment & Camera
- **`Background`, `Cloud`, `Decoration`, `Platform`**: Các lớp thực thể quản lý việc vẽ hình nền, nền tảng vật lý (có xử lý va chạm), mây trôi và các phụ kiện trang trí trong World.
- **`TileMap`**: Quản lý load và render map LDtk, parse entity data.
- **`MapCamera`**: Quản lý camera 2D, hỗ trợ single/multiplayer và State Pattern qua các Camera Mode.
- **`ICameraMode`**: Interface cho các camera mode (State Pattern).
- **`CameraFollowMode`**: Mode mặc định bám theo 1 hoặc 2 người chơi.
- **`CameraPanMode`**: Mode camera trượt mượt mà đến tọa độ chỉ định (Cinematic).
- **`CameraZoomMode`**: Mode camera zoom mượt mà đến tỷ lệ phóng đại chỉ định (Cinematic).

#### Cutscene & Dialogue
- **`DialogueData`**: Chứa struct dữ liệu `DialogueSequence` và `DialogueLine` (Mô hình Dữ liệu thuần).
- **`DialogueLoader`**: Chịu trách nhiệm parse các file JSON dialogue.
- **`DialogueRegistry`**: Singleton lưu trữ các dialogue đã parse để tái sử dụng nhanh chóng.
- **`DialogueBox`**: Vẽ hộp thoại kiểu RPG, quản lý Typewriter effect.
- **`CutsceneScript`**: Struct chứa tham số của 1 cutscene (dialogueId, cameraPanTarget...).
- **`CutsceneTrigger`**: Region trên map load từ LDtk, kích hoạt Cutscene khi Player đi vào.
- **`CutsceneManager`**: Orchestrator điều phối Camera Spanning và DialogueBox (quản lý 4 Phase: PanIn, Dialogue, PanBack, Done). Block input người chơi nhưng giữ game running.

#### Items
- **`BaseItem`**: Abstract class cho các vật thể tương tác tĩnh (không có gravity) như Coin, Rương. Nó cung cấp hàm `drawAnim` hỗ trợ hiển thị Animation.
- **`AtlasAnimation`**: Quản lý Animation cho các Item tĩnh bằng cách cắt Rectangle toán học dựa trên một Texture chung. Hỗ trợ sprite strips trên Atlas.
- **`ItemAtlasRegistry`**: Flyweight Singleton lưu một Texture khổng lồ (`a.png`) cho mọi Item để tối ưu Draw Calls.
- **`Coin`, `Chest`, `Spring`, ...**: Các class Item cụ thể kế thừa từ `BaseItem`.

#### UI
- **`HUD`**: Lớp chứa logic in thông tin lên màn hình (Máu, Điểm, Thời gian).
- **`OverlayUI`**: Interface cho giao diện hiển thị đè lên màn chơi (Settings, Chọn nhân vật), hỗ trợ `Render(float alpha) const`.
- **`SettingsOverlay`**: Menu tùy chỉnh đè lên World (Save, Load, Về Menu chính).
- **`CharacterSelectionOverlay`**: UI cho phép 2 người chơi swap chọn nhân vật đè lên World (Mario, Luigi, v.v.).
- **`UIComponent`**: Lớp cơ sở (như `Button`) phục vụ cho các state tĩnh (Intro, Setting) để dễ dàng thao tác bấm nút.

#### Root & Misc Folders
- **`assets/`**: Chứa các tài nguyên của game (âm thanh, hình ảnh, font chữ...). Hiện tại đang trống.
- **`build/`, `build_mingw/`**: Thư mục chứa các file cấu hình và executable sinh ra từ quá trình build (CMake).
- **`third_party/`**: Chứa các thư viện bên thứ 3, cụ thể là `raylib`.
- **`workflow/`**: Chứa các file tài liệu thiết kế hệ thống (UML diagrams) và kế hoạch dự án (`PLAN.md`).
- **`CMakeLists.txt`, `CMakePresets.json`**: Cấu hình build dự án sử dụng CMake.
- **`.cursorrules`**: Các luật cấu hình cho AI / Cursor.

---
*Note: History of monolithic structure has been pruned to keep documentation concise as per rules.*
