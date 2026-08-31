# SuperMarioPlus Project Structure & Progress

**Project Purpose:** A 2D platformer game (Super Mario style) built using C++ and Raylib.

## Tình trạng & Tiến độ dự án hiện tại (Current Status & Progress)
- **Kiến trúc:** Dự án đã được refactor toàn diện theo nguyên tắc OOP (1 file/class) và phân chia thành các module rõ ràng (`abilities`, `characters`, `core`, `environment`, `states`, `ui`, `editor`).
- **Design Patterns:** Đã áp dụng **State Pattern** cho quản lý vòng lặp game (`GameState`, `CharacterState`), **Strategy Pattern** cho hệ thống kỹ năng (`AbilityStrategy`) và công cụ editor (`IEditorTool`), cùng với **Factory Pattern** cho khởi tạo nhân vật (`CharacterFactory`) và vật phẩm (`ItemFactory`).
- **Hoàn thiện:** Hệ thống quản lý màn hình (Menu, World), vẽ map (LDtk parser, TileMap), entity (Player, Quái, Items), combat system cơ bản và hệ thống state nhân vật. Gần đây nhất đã hoàn thành tính năng **In-game Map Editor (Level Editor)** cho phép người chơi tự thiết kế map (đặt block, entity, thay đổi kích thước map) và Test Play trực tiếp. Hệ thống Save/Load Custom Map dưới dạng JSON cũng đã được tích hợp.
- **Đang dở dang (WIP):** Tối ưu thêm về hiệu năng và bổ sung các màn chơi mới, sửa lỗi phát sinh. Logic chi tiết của vài tính năng UI Settings mở rộng.
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
│   ├── editor/
│   │   ├── EditorBlockDef.h
│   │   ├── EditorBlockRegistry.h
│   │   ├── CustomEntityData.h
│   │   ├── CustomMapData.h
│   │   ├── CustomMapSerializer.h
│   │   ├── IEditorTool.h
│   │   ├── PlaceTileTool.h
│   │   ├── PlaceEntityTool.h
│   │   ├── EraseTool.h
│   │   ├── EditorToolManager.h
│   │   ├── EditorCamera.h
│   │   ├── EditorMapResizer.h
│   │   ├── CategoryPanel.h
│   │   ├── BlockVariantPanel.h
│   │   ├── EditorBottomPanel.h
│   │   └── UndoRedoStack.h
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
│   │   ├── CharacterSelectionState.h
│   │   ├── CharacterState.h
│   │   ├── GameState.h
│   │   ├── IntroState.h
│   │   ├── LoadingState.h
│   │   ├── MapSelectionState.h
│   │   ├── Menu.h
│   │   ├── PlayerStates.h
│   │   ├── SettingState.h
│   │   └── World.h
│   └── ui/
│       ├── transitions/
│       │   ├── ITransition.h
│       │   └── IrisTransition.h
│       ├── CharacterSelectionOverlay.h
│       ├── HUD.h
│       ├── PlayerHUD.h
│       ├── IngameSettingsPanel.h
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
│   │   ├── CharacterSelectionState.cpp
│   │   ├── GameState.cpp
│   │   ├── IntroState.cpp
│   │   ├── LoadingState.cpp
│   │   ├── MapSelectionState.cpp
│   │   ├── Menu.cpp
│   │   ├── PlayerStates.cpp
│   │   ├── SettingState.cpp
│   │   └── World.cpp
│   └── ui/
│       ├── transitions/
│       │   └── IrisTransition.cpp
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
- **`LoadingState`**: State đóng vai trò như một wrapper, hiển thị thanh tiến trình loading và đếm thời gian trước khi chuyển sang state đích (State Pattern).
- **`MenuState` (`Menu`)**: Màn hình chọn nhân vật và bắt đầu game.
- **`MapSelectionState`**: Màn hình chọn map hiển thị `map_selection.png` và xử lý hiệu ứng mở rộng vòng tròn.
- **`CharacterSelectionState`**: Màn hình chọn nhân vật độc lập, sử dụng Factory Lambda để cho phép chọn nhân vật linh hoạt trước khi load vào World hoặc Custom Map.
- **`World1_1State` (`World`)**: Quản lý màn hình chơi chính (Mario, Quái, Môi trường). Bắt các OverlayUI khi ấn phím đặc biệt.
- **`SettingState`**: Màn hình thiết lập hệ thống từ Intro.
- **`CharacterState` / `PlayerStates`**: Base interface và các lớp triển khai cho State Pattern của nhân vật (Idle, Running, Jumping).

#### Characters
- **`Character` / `Player`**: Base class đại diện cho nhân vật. Player kế thừa Character, chứa input và xử lý logic riêng. (Đã tích hợp hệ thống Oxygen/Thở dưới nước bằng Accumulator trong `update`).
- **`CharacterStats`**: Chứa các Struct phân định rạch ròi Data-Driven (Base, Runtime, World). `CharacterRuntimeStats` quản lý các biến tích lũy (Accumulator Timer) cho hệ thống tiêu hao Oxy.
- **`CharacterFactory`**: Factory Pattern để tự động sinh ra các nhân vật dựa trên chuỗi định danh (Tên).
- **`Mario, Luigi, Peach, Toad, Wario`**: Các lớp cụ thể đại diện cho người chơi, được override các kỹ năng tương ứng.
- **`Goomba`**: Kẻ thù cơ bản, kế thừa từ `Character`.
- **`Boss`**: Kế thừa `Mob`, hỗ trợ cơ chế Cutscene ID (Observer pattern) thông qua hàm `onCutsceneStart` và `onCutsceneEnd` để đồng bộ Boss Intro.
- **Enemy States (`EnemyIdleState`, `EnemyRunState`, `EnemyAttackState`, `EnemyHurtState`, `EnemyDieState`, `EnemySkillState`)**: Các state AI của kẻ địch (Mob), hỗ trợ tự động tìm Player (distance check), tạo hitbox qua `CombatSystem`, xử lý đẩy lùi (knockback). Quái (Mob/Boss) được trang bị hệ thống `Edge/Water Detection` trong `Entity::checkEdgeAndWater` giúp chúng tự động quay đầu khi gặp mép vực hoặc mép nước (tránh rơi xuống nước). Đặc biệt `EnemySkillState` hỗ trợ random xuất chiêu dựa trên `IEnemySkill`.
- **Boss States (`BossIdleState`, `BossIntroState`, `BossPatrolState`, `BossRunState`, `BossHurtState`, `BossAttackState`, `BossDieState`, `BossSkillState`, `BossDebugInputState`)**: Các state chuyên biệt dành cho Boss. Trong đó `Idle/Intro` xử lý logic chờ Cutscene, còn lại dùng để quản lý AI (tìm đường, chọn skill ngẫu nhiên khi tấn công) cho toàn bộ các Boss trong game (Data-Driven qua file cấu hình). `BossSkillState` hỗ trợ tự động load hitbox và animation của từng chiêu riêng biệt tương tự như `EnemySkillState`. `BossDebugInputState` cho phép tắt AI để người chơi tự tay điều khiển boss thủ công.

#### Abilities & Skills (Strategy Pattern)
- **`AbilityStrategy`**: Interface định nghĩa `Execute()` cho các kỹ năng của Player.
- **`DashAbility`, `FireballAbility`, `FloatAbility`, `GroundPoundAbility`, `HighJumpAbility`**: Các lớp triển khai kỹ năng cụ thể cho từng nhân vật. Gắn linh hoạt vào Character.
- **`IEnemySkill`**: Interface độc lập (Decoupled) dùng riêng cho AI của Quái/Boss. Cung cấp hàm `execute(Mob&)` cùng với logic tính toán hitbox, damage, thời gian thi triển.
- **`BasicMeleeEnemySkill`**: Kỹ năng mẫu cơ bản dành cho Quái (tự động gây sát thương nếu Player nằm trong tầm đánh).

#### Editor
- **`MapEditorState`**: GameState chính quản lý toàn bộ level editor (vẽ map, ghost preview, panels, input loop).
- **`CustomMapData`**: Cấu trúc dữ liệu sparse (chỉ lưu các block/entity thực tế có) để lưu trữ map do user tạo.
- **`CustomMapSerializer`**: Chịu trách nhiệm ghi/đọc `CustomMapData` ra định dạng JSON (thư mục `saves/`).
- **`EditorBlockRegistry`**: Quản lý và cung cấp thông tin metadata, texture, fallback color cho tất cả các loại block (đất, đá, mây...) trích xuất từ world01->06.
- **`EditorToolManager` & `IEditorTool`**: Pattern Strategy quản lý các công cụ Place Tile, Place Entity, và Erase.
- **UI Panels (`EditorBottomPanel`, `CategoryPanel`, `BlockVariantPanel`)**: Quản lý giao diện, hiển thị các category và grid icon cho user chọn block.
- **`UndoRedoStack`**: Hệ thống undo/redo lưu snapshot toàn bộ map data (lightweight vì là sparse data).
- **`EditorCamera` & `EditorMapResizer`**: Xử lý việc cuộn, thu phóng camera bằng chuột giữa và các nút kéo biên (handle) để mở rộng/thu hẹp map.

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

#### Items & Buffs
- **`BaseItem`**: Abstract class cho các vật thể tương tác tĩnh (không có gravity) như Coin, Rương. Nó cung cấp hàm `drawAnim` hỗ trợ hiển thị Animation.
- **`AtlasAnimation`**: Quản lý Animation cho các Item tĩnh bằng cách cắt Rectangle toán học dựa trên một Texture chung. Hỗ trợ sprite strips trên Atlas.
- **`ItemAtlasRegistry`**: Flyweight Singleton lưu một Texture khổng lồ (`a.png`) cho mọi Item để tối ưu Draw Calls.
- **`Coin`, `Chest`, `Spring`, ...**: Các class Item cụ thể kế thừa từ `BaseItem`.
- **`Buff` & `BuffManager`**: `Buff` là vật phẩm rơi ra từ LuckyBlock/Chest. Khi người chơi sử dụng qua `ConsumeBuffStrategy`, nó được add vào `BuffManager`.
- **`IBuffEffect` & `HealBuff`, `SpeedBuff`,...**: Interface và các triển khai cụ thể cho tác dụng của Buff. Mới thêm `HealBuff` (hồi máu tức thì cho Player).

#### UI
- **`HUD` / `PlayerHUD`**: Hiển thị avatar tròn, thanh HP/MP/Breath và ô chứa Item của người chơi.
- **`IngameSettingsPanel`**: Giao diện Settings in-game (Controls, Sounds, Quit to Menu) được kích hoạt qua nút bánh răng cưa (cogwheel) ở góc trên bên trái trong 6 World và Custom Map.
- **`OverlayUI`**: Interface cho giao diện hiển thị đè lên màn chơi (Settings, Chọn nhân vật), hỗ trợ `Render(float alpha) const`.
- **`SettingsOverlay`**: Menu tùy chỉnh đè lên World (Save, Load, Về Menu chính).
- **`CharacterSelectionOverlay`**: UI cho phép 2 người chơi swap chọn nhân vật đè lên World (Mario, Luigi, v.v.).
- **`UIComponent`**: Lớp cơ sở (như `Button`) phục vụ cho các state tĩnh (Intro, Setting) để dễ dàng thao tác bấm nút.
- **`ITransition` & `IrisTransition`**: Interface và class triển khai hệ thống hiệu ứng chuyển cảnh (Strategy Pattern) bằng cách vẽ lớp mặt nạ vòng tròn.

#### Root & Misc Folders
- **`assets/`**: Chứa các tài nguyên của game (âm thanh, hình ảnh, font chữ...). Hiện tại đang trống.
- **`build/`, `build_mingw/`**: Thư mục chứa các file cấu hình và executable sinh ra từ quá trình build (CMake).
- **`third_party/`**: Chứa các thư viện bên thứ 3, cụ thể là `raylib`.
- **`workflow/`**: Chứa các file tài liệu thiết kế hệ thống (UML diagrams) và kế hoạch dự án (`PLAN.md`).
- **`CMakeLists.txt`, `CMakePresets.json`**: Cấu hình build dự án sử dụng CMake.
- **`.cursorrules`**: Các luật cấu hình cho AI / Cursor.

---
*Note: History of monolithic structure has been pruned to keep documentation concise as per rules.*
