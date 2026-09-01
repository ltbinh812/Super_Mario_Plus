# SuperMarioPlus Project Structure & Progress

**Project Purpose:** A 2D platformer game (Super Mario style) built using C++ and Raylib.

## Tình trạng & Tiến độ dự án hiện tại (Current Status & Progress)
- **Kiến trúc:** Dự án đã được refactor toàn diện theo nguyên tắc OOP (1 file/class) và phân chia thành các module rõ ràng (`abilities`, `characters`, `core`, `environment`, `states`, `ui`, `editor`).
- **Design Patterns:** Đã áp dụng **State Pattern** cho quản lý vòng lặp game (`GameState`, `CharacterState`), **Strategy Pattern** cho hệ thống kỹ năng (`AbilityStrategy`) và công cụ editor (`IEditorTool`), cùng với **Factory Pattern** cho khởi tạo nhân vật (`CharacterFactory`) và vật phẩm (`ItemFactory`).
- **Hoàn thiện:** Hệ thống quản lý màn hình (Menu, World), vẽ map (LDtk parser, TileMap), entity (Player, Quái, Items), combat system cơ bản và hệ thống state nhân vật. Gần đây nhất đã hoàn thành tính năng **In-game Map Editor (Level Editor)** cho phép người chơi tự thiết kế map (đặt block, entity, thay đổi kích thước map) và Test Play trực tiếp. Hệ thống Save/Load Custom Map dưới dạng JSON cũng đã được tích hợp.
- **Mới nhất — Hệ thống Save/Load nhiều version (1-Player):** Đã hoàn thành module `include/save/` + `src/save/` theo kiến trúc 4 tầng (Strategy `ISaveSerializer`, Repository `ISaveRepository`, Facade `SaveManager`, Registry `WorldCatalog`). Mỗi lần chạm `Flag` sinh một file `saves/world0X/versionN.json` lưu đầy đủ trạng thái người chơi (máu/mana/coin/key/item đang cầm/nhân vật) và trạng thái map (IID quái đã chết, IID item đã đổi trạng thái, level hiện tại). Ở Map Selection, bấm một world sẽ hiện popup `NEW GAME` / `LOAD GAME`; `LOAD GAME` mở panel danh sách bản lưu, chọn một dòng thì hiện `DELETE` và `LOAD`. Xem mục **Save System** bên dưới.
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
│   ├── save/                          # [MỚI] Hệ thống Save/Load nhiều version
│   │   ├── PlayerSaveData.h           #   DTO: trạng thái 1 người chơi
│   │   ├── InventorySaveData.h        #   DTO: coin + key dùng chung
│   │   ├── LevelSaveData.h            #   DTO: quái đã chết + item đã đổi trạng thái
│   │   ├── SaveMetaData.h             #   DTO: "bìa sách" cho panel liệt kê
│   │   ├── GameSaveData.h             #   Aggregate Root gom 4 DTO trên
│   │   ├── SaveSlotInfo.h             #   Mô tả 1 file save trên đĩa
│   │   ├── ISaveSerializer.h          #   Strategy: định dạng lưu trữ
│   │   ├── JsonSaveSerializer.h       #   Strategy impl: nlohmann/json
│   │   ├── ISaveRepository.h          #   Repository: kho chứa bản lưu
│   │   └── FileSaveRepository.h       #   Repository impl: std::filesystem
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
│   │   ├── World.h
│   │   ├── WorldDescriptor.h          # [MỚI] Hồ sơ 1 world (path + 2 factory)
│   │   └── WorldCatalog.h             # [MỚI] Registry 6 world (thay switch(idx))
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
│       ├── UIComponent.h
│       ├── PanelButton.h              # [MỚI] Nút bấm dùng chung cho panel overlay
│       ├── WorldActionPanel.h         # [MỚI] Popup NEW GAME / LOAD GAME
│       ├── SaveVersionPanel.h         # [MỚI] Danh sách bản lưu + DELETE/LOAD
│       └── UIScaler.h                 # [MỚI] Quy đổi khung thiết kế ảo -> màn hình thật
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
│   ├── save/                          # [MỚI]
│   │   ├── JsonSaveSerializer.cpp
│   │   └── FileSaveRepository.cpp
│   ├── states/
│   │   ├── CharacterSelectionState.cpp
│   │   ├── GameState.cpp
│   │   ├── IntroState.cpp
│   │   ├── LoadingState.cpp
│   │   ├── MapSelectionState.cpp
│   │   ├── Menu.cpp
│   │   ├── PlayerStates.cpp
│   │   ├── SettingState.cpp
│   │   ├── World.cpp
│   │   └── WorldCatalog.cpp           # [MỚI]
│   └── ui/
│       ├── transitions/
│       │   └── IrisTransition.cpp
│       ├── CharacterSelectionOverlay.cpp
│       ├── HUD.cpp
│       ├── PlayerHUD.cpp
│       ├── SettingsOverlay.cpp
│       ├── UIComponent.cpp
│       ├── PanelButton.cpp            # [MỚI]
│       ├── WorldActionPanel.cpp       # [MỚI]
│       ├── SaveVersionPanel.cpp       # [MỚI]
│       └── UIScaler.cpp               # [MỚI]
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

#### Save System (`include/save/`, `src/save/`) — Lưu game nhiều version cho chế độ 1-Player

**Mục tiêu:** mỗi lần người chơi chạm `Flag` (checkpoint) sẽ sinh thêm một file
`saves/world0X/versionN.json` ghi lại đầy đủ trạng thái người chơi và trạng thái map.
Ở màn Map Selection, bấm một world sẽ hiện popup `NEW GAME` / `LOAD GAME`.

**Kiến trúc 4 tầng** (mỗi tầng một trách nhiệm, ghép với nhau bằng Dependency Injection):

```
BaseLevelState ──createSaveData()──> GameSaveData
       │                                  │
       v                                  v
  SaveManager  ───────────────>  ISaveRepository  ──uses──>  ISaveSerializer
   (Facade,                     (FileSaveRepository)         (JsonSaveSerializer)
    Singleton)                   quản lý thư mục               struct <-> JSON
                                 + đánh số version
```

| File / Class | Tác dụng | Được dùng bởi | Cần gì bổ trợ |
|---|---|---|---|
| `PlayerSaveData` | DTO: nhân vật, máu, mana, breath, vị trí, hướng nhìn, item đang cầm | `Player::createSaveData()` | — |
| `InventorySaveData` | DTO: coin + key (ánh xạ `PartyInventory`) | `BaseLevelState` | — |
| `LevelSaveData` | DTO: tập IID quái đã chết + map IID→`ItemState` + level hiện tại | `BaseLevelState`, `spawnEntitiesFromMap()` | `ItemState.h` |
| `SaveMetaData` | DTO "bìa sách": nhân vật, level, coin, máu, giờ chơi, thời điểm lưu | `SaveVersionPanel` để vẽ danh sách mà không phải parse cả file | — |
| `GameSaveData` | Aggregate Root gom 4 DTO trên | Toàn hệ thống save | — |
| `SaveSlotInfo` | Mô tả **một file trên đĩa** (đường dẫn + meta), chưa nạp nội dung | `SaveVersionPanel`, `MapSelectionState` | — |
| `ISaveSerializer` | **Strategy**: định dạng lưu trữ | `FileSaveRepository`, `SaveManager` | — |
| `JsonSaveSerializer` | Cài đặt bằng nlohmann/json; dùng `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT` để sinh **cả** `to_json` lẫn `from_json` từ một danh sách trường → không thể lệch ghi/đọc | `SaveManager` | `third_party/json` |
| `ISaveRepository` | **Repository**: liệt kê / tạo / nạp / xoá bản lưu của một world | `SaveManager` | — |
| `FileSaveRepository` | Cài đặt trên `std::filesystem`; bố cục `saves/world0X/versionN.json`; tự tạo thư mục; số version luôn tăng, không tái dùng; bỏ qua file hỏng | `SaveManager` | `ISaveSerializer` (tiêm qua ctor) |
| `SaveManager` *(mở rộng)* | **Facade + Singleton**. Hai vai trò: (1) checkpoint trong RAM để hồi sinh — API cũ giữ nguyên; (2) bản lưu trên đĩa qua `listVersions/createVersion/loadVersion/deleteVersion` | `BaseLevelState`, `MapSelectionState` | `ISaveRepository` |
| `include/core/SaveData.h` | Nay chỉ là **aggregator header** include 6 file trên — mọi call site cũ vẫn biên dịch được, không xoá gì | tương thích ngược | — |

**Registry ánh xạ world:**

| File / Class | Tác dụng | Được dùng bởi |
|---|---|---|
| `WorldDescriptor` | Hồ sơ 1 world: số hiệu, tên hiển thị, đường dẫn `.ldtk`, và 2 factory (`makeNew`, `makeLoaded`) | `WorldCatalog` |
| `WorldCatalog` | **Registry + Factory**, nơi DUY NHẤT ánh xạ `worldIndex ↔ World0XState` — thay cho khối `switch(idx)` 6 nhánh trước đây trong `MapSelectionState`. `indexFromMapPath()` trả `-1` cho `menu.ldtk`/`pvp_map0N`/custom map, nhờ đó auto-save tự động tắt ở mọi ngữ cảnh không phải world 1-player | `MapSelectionState`, `BaseLevelState::onCheckpointReached()` |

**UI overlay:**

| File / Class | Tác dụng | Được dùng bởi |
|---|---|---|
| `PanelButton` | Nút bấm có texture + font riêng + hiệu ứng hover. `ConsumeClick()` trả sự kiện ra ngoài thay vì tự gọi callback → giữ đúng phân tách 4 giai đoạn | `WorldActionPanel`, `SaveVersionPanel` |
| `WorldActionPanel` (`IMenuPanel`) | Popup hiện đè lên Map Selection sau khi bấm một world: `NEW GAME` / `LOAD GAME` / `BACK` | `MapSelectionState` |
| `SaveVersionPanel` (`IMenuPanel`) | Danh sách bản lưu (cuộn được, mới nhất trên đầu). Chọn một dòng thì mới hiện `DELETE` và `LOAD`. Danh sách rỗng → hiện `NO SAVED GAME` | `MapSelectionState` |

**Luồng người dùng:**
```
MapSelectionState
  └─ bấm world N ──> WorldActionPanel (overlay)
       ├─ NEW GAME  ──> LoadingState ─> CharacterSelectionState ─> WorldCatalog::createNew(N, p1)
       └─ LOAD GAME ──> SaveVersionPanel (overlay)
              └─ chọn version ─> [DELETE] xoá file rồi nạp lại danh sách
                              └─ [LOAD]  ─> LoadingState ─> WorldCatalog::createLoaded(N, save)
```

#### Ảnh động GIF (`include/infrastructure/GifAnimation.h`)

`LoadTexture("x.gif")` của raylib chỉ nạp khung hình ĐẦU TIÊN nên ảnh đứng im.
`GifAnimation` bọc lại cặp API đúng (`LoadImageAnim` + `UpdateTexture` theo offset
`width*height*4*frameIndex`) thành một object tự chạy hoạt ảnh.

| Thành phần | Ghi chú |
|---|---|
| `GifAnimation` | Sở hữu cả `Image` (RAM) lẫn `Texture2D` (VRAM); cấm copy, cho phép move. `Update(dt)` tiến khung, `DrawFullscreen()` / `DrawPro()` chỉ vẽ |
| Dùng ở `CharacterSelectionState` | Nền động `assets/UI_screens/character_selection.gif` (1400x787, 14 khung, ~6.7 fps). Không nạp được thì tự rơi về ảnh tĩnh `map_selection.png`. Phủ thêm lớp đen `alpha=128` (50%) ngay sau khi vẽ nền để tiêu đề và thẻ nhân vật nổi lên |
| Dùng ở `EndgameAsset` | Cổng kết thúc `assets/maps/item/gate.gif` (320x320, 9 khung, 10 fps). Đồng thời sửa đường dẫn tuyệt đối `d:/Git/...` thành tương đối, và chuyển việc tiến khung hình từ `render()` sang `update(dt)` cho đúng quy tắc 4 giai đoạn |

> ⚠ **stb_image trong raylib 5.0 có thể SẬP (segfault) với một số file GIF.**
> `gate.gif` bản gốc làm crash `stbi_load_gif_from_memory`, phải mã hoá lại bằng
> Pillow. Bản gốc giữ ở `assets/maps/item/gate.gif.original`. Đây là lỗi trong
> thư viện, không bắt được bằng try/catch — nếu thêm GIF mới mà game tắt ngay
> khi vào màn, hãy mã hoá lại file đó qua Pillow trước.
>
> **KHI MÃ HOÁ LẠI GIF PHẢI GIỮ KÊNH TRONG SUỐT.** GIF không có kênh alpha thật;
> nó đánh dấu trong suốt bằng cách dành riêng MỘT ô trong bảng màu
> (`transparency=<index>`). Nếu chỉ làm `convert('RGB')` rồi `quantize()` thì ô
> đó biến mất và toàn bộ vùng trong suốt thành **màu đen đục**. Quy trình đúng:
> dựng bảng màu 255 ô chỉ từ pixel ĐỤC, dành index 255 cho trong suốt, `paste()`
> index đó vào vùng `alpha < 128`, rồi lưu kèm `transparency=255, disposal=2`.
> Bản `gate.gif` hiện tại đã kiểm chứng: sai lệch màu 0/255, sai lệch alpha 0/255,
> đúng 75268/102400 pixel trong suốt như bản gốc; raylib giải mã ra 0 pixel
> đen-đục. Còn 9 khung vì khung 10 của bản gốc trùng hệt khung 9.

#### Bố cục độc lập độ phân giải (`include/ui/UIScaler.h`)

`MainMenuState` trước đây viết thẳng hằng số pixel (`panelScale = 3.3f`,
`paddingLeft = 220.0f`, toggle ở `{30, 90}`) canh mắt trên một màn hình cụ thể;
sang máy khác là lệch. `UIScaler` áp dụng mô hình **khung thiết kế ảo**:

```
factor  = min(screenW / 2560, screenH / 1440)     // min -> không bóp méo ảnh
offsetX = (screenW - 2560*factor) / 2             // canh giữa phần dư
```

- `S(len)` đổi **độ dài / hệ số phóng**; `X()`, `Y()`, `Pos()`, `Rect()` đổi **toạ độ** (có cộng offset). Nhầm hai nhóm này là lỗi bố cục hay gặp nhất.
- Khung thiết kế là **2560x1440**, KHÔNG phải `InitWindow(1280, 720)` trong `main.cpp`: cửa sổ được `MaximizeWindow()` ngay sau đó, và raylib đếm **pixel vật lý** (màn 2560x1600 ở DPI 200% -> raylib báo 2560x1459, trong khi Windows báo 1280x800 logic).
- Ở 2560x1459 thì `factor = 1.0` -> giao diện giữ nguyên từng pixel như trước khi sửa; các độ phân giải khác scale theo đúng tỉ lệ.
- **Giới hạn đã biết:** bố cục được tính một lần trong constructor. Cửa sổ có cờ `FLAG_WINDOW_RESIZABLE`, nên nếu người chơi kéo giãn cửa sổ giữa chừng thì menu không tự dàn lại (phải gọi `ui_.Refresh()` rồi dựng lại layout).

#### Màn hình kết thúc (`include/states/EndgameState.h`)

| Chế độ | Ảnh nền | Nội dung |
|---|---|---|
| 1-Player | `level_completed_background.png` | Nút quay về Menu ở góc dưới phải |
| 2-Player | `winner_background.png` | Nút quay về Menu + **hoạt ảnh nhân vật thắng đứng trên bục podium** |

- **Lỗi đã sửa:** toàn bộ phần nạp tài nguyên nằm trong `Init()`, nhưng `GameState` không có `Init()` trong interface và `StateManager` không bao giờ gọi nó → `bgTex_` rỗng, `screenW_/screenH_` bằng 0, nút quay về co thành hình 0x0 không bấm được, màn hình chỉ hiện chữ `"Game Ended!"`. Nay `Init()` được gọi trong constructor như mọi state khác, và tự `Cleanup()` trước nên gọi lại vẫn an toàn.
- **Nút quay về:** kích thước `10%` chiều cao khung hình, lề `5%` mỗi cạnh → đúng góc dưới phải ở mọi độ phân giải. Vùng bấm giữ nguyên kích thước lúc hover còn vùng vẽ mới co lại, tránh nút rung ở mép (mẹo lấy từ `MapSelectionState`).
- **Hoạt ảnh người thắng:** tái dùng khuôn mẫu island của `CharacterSelectionState` — đọc `characters.json`, nạp `idle` + `attack_1` qua `AssetManager` (Flyweight, không tốn thêm VRAM), idle lặp vô hạn, cứ 3 giây có 50% cơ hội chen một lượt kỹ năng. Neo **giữa-đáy** tại `(0.50, 0.672)` của khung hình = mặt bục podium, cao `26%` chiều cao khung hình, rộng suy ra theo tỉ lệ ảnh nên không bóp méo. Vì ảnh nền được kéo giãn phủ kín màn hình, toạ độ theo tỉ lệ này đúng ở mọi khung hình.
- **Điều kiện thắng PvP (mới):** trước đây PvP **không có điều kiện kết thúc nào** — `EndgameState` chỉ được tạo từ cổng `EndgameAsset` của luồng 1-Player, còn `processDeathCondition()` chỉ kiểm tra player1. Nay `BaseLevelState::processDeathCondition()` có nhánh PvP riêng: ai gục trước thì bên kia thắng, không hồi sinh; nếu rơi ra ngoài map mà máu còn thì `takeDamage(9999)` để animation chết chạy; đợi `pvpEndTimer_` = 1.5s rồi mới chuyển sang màn trao giải kèm tên người thắng. Hai bên cùng gục trong một frame → hoà, không hiện nhân vật nào.

#### Custom Map — đợt sửa lỗi & tái cấu trúc

**Quy trình dữ liệu (đọc kỹ trước khi sửa):**
```
file .ldtk gốc --(offline, 1 lần)--> EditorBlockRegistry + extracted_rules.json
người chơi dựng map trong editor    --> CustomMapData (RAM)
Save                                --> saves/custom_map_N.json
Load                                --> CustomMapData --> TileMap::LoadCustomMap
```
Luồng **không** đi ngược ra file `.ldtk`. Hàm `CustomMapSerializer::exportToLDtk` (513 dòng, 76% file) đã bị **gỡ bỏ**: nó ghi ra `saves/custom_map_N.ldtk` mà không một dòng code nào trong dự án nạp lại, và bản thân nó sinh sai dữ liệu (map 20 tile xuất ra 887 tile, 74 ô nằm ngoài biên level, mỗi ô viền bị lặp trên 7 tileset khác nhau, thứ tự layer ngẫu nhiên theo `unordered_map`). Đừng thêm lại trừ khi thật sự có bên tiêu thụ.

**File mới:**

| File | Vai trò |
|---|---|
| `include/editor/CustomMapValidator.h` + `.cpp` | Luật hợp lệ của map, tách khỏi `MapEditorState`. Hợp lệ ⟺ (1 PlayerSpawn + đúng 1 `Boss_*`) hoặc (2 PlayerSpawn + 0 boss). Trả kèm `PlayMode` để suy ra số người chơi và cờ PvP từ chính dữ liệu map |
| `include/editor/IconFit.h` | Đặt vùng ảnh vào ô đích giữ nguyên tỉ lệ. Gom **5 bản chép** của cùng một đoạn toán (2 nhánh ghost, `TileMap`, `BlockVariantPanel`, `EntityPalette`) |
| `include/editor/EditorToolType.h`, `SaveLoadMode.h` | Tách enum khỏi header của class theo quy tắc 1 khai báo / 1 file |

**Lỗi đã sửa (nhóm gây sập / mất dữ liệu):**

| Chỗ | Lỗi |
|---|---|
| `TileMap::LoadCustomMap` | `reg.getCollision()` gọi không kiểm tra `has()` → `std::out_of_range` không ai bắt → tắt game khi nạp map có blockId đã đổi tên |
| `TileMap::LoadLDtkMap` | `playerSpawns.clear()` chạy **trước** khi kiểm tra mở được file → nạp hỏng là xoá sạch dữ liệu map đang chạy |
| `BaseLevelState::processDeathCondition` | Chết trong custom map: hoặc văng sang world khác (checkpoint là singleton, `clearCheckpoint()` chưa từng được gọi), hoặc hồi sinh ở toạ độ cứng `{180,208}` giữa hư không. Nay có nhánh riêng: hồi sinh tại spawn của chính map |
| `EditorMapResizer` | Không có `MAX_SIZE` → một cú kéo ở zoom thấp phóng map lên hàng nghìn ô → treo. Nay kẹp `[MIN_SIZE, MAX_SIZE=500]` |
| `AutoTiler::loadRules` | `try` chỉ bọc `file >> j`; `stoi`/`get<int>()`/`operator[]` const nằm ngoài → file rules hỏng là tắt game **lúc vào editor** |
| `CustomMapSerializer::load` | `operator[]` const trên phần tử mảng: với nlohmann 3.11.3 + NDEBUG là deref `end()` (UB), không phải exception. Nay dùng `.value()` toàn bộ + kẹp `width/height/tileSize` + lọc key ngoài biên |
| `CustomMapSerializer::save` | Không kiểm tra `ofstream`, luôn `return true`. Nay ghi file tạm rồi `rename` (atomic) |

**Lỗi đã sửa (nhóm chức năng không dùng được):**

| Chỗ | Lỗi |
|---|---|
| `EditorBottomPanel` | Bố cục dùng pixel tuyệt đối, phần cố định 1168px → ở cửa sổ 1280 vùng chọn block chỉ còn 112px (icon rộng 128px), ở ≤1168px thì **âm** và không chọn được gì. Nay đi qua `UIScaler`, và `render`/`handleInput` dùng chung một `computeLayout()` thay vì mỗi hàm tự tính lại |
| `MapEditorState::handleLoad` | Không hề gán `mapData_` → map đã lưu **không bao giờ mở lại sửa được**. Nay LOAD nạp thật (kèm `undoRedo_.clear()`), và có nút **PLAY** riêng để chơi thử |
| `TileMap::LoadCustomMap` | Toạ độ entity dùng góc trên-trái trong khi `BaseItem` cần góc dưới-trái → mọi item lệch lên đúng 1 ô so với vị trí đặt trong editor |
| `PlaceEntityTool` | `fields` lưu dạng object nhưng `ItemFactory` chỉ đọc `is_array()` → mọi Buff đặt trong editor đều thành `RandomBuff` |
| `BaseLevelState` ctor custom | Chép tay ~60 dòng từ ctor LDtk và đã trôi: không spawn quái, không khôi phục trạng thái item, không cutscene trigger, không `bindPlayerInputs()`, và **luôn ép PvP** khi có 2 người. Nay cả hai ctor dùng chung `initWorldFromLoadedMap()`, `isPvPMode` là tham số |
| `MapEditorState` undo | Ghi mốc ngay khi *nhấn chuột*, chưa biết có đổi gì → lịch sử 50 bước bị lấp đầy bản chụp rỗng. Nay `IEditorTool` trả `bool`, chỉ ghi khi thật sự đổi. Resize cũng đã vào undo |
| `EditorBottomPanel` | Bấm category âm thầm reset tool về Place (đang xoá thành ra vẽ). Nút Back nằm trong vùng map và không nuốt click → bấm Back vừa thoát vừa đặt một block |
| `EntityPalette` | Icon `Spring_left`/`Spring_right` hoán đổi; `scrollOffsetX_` khai báo `int` nhưng gán biểu thức thực |

**Refactor OOP / Design Pattern:**

- **`IEditorTool` thêm `renderGhost()`** — chuỗi `if/else` 80 dòng rẽ theo enum tool trong `MapEditorState` (vi phạm đóng-mở: thêm tool là phải sửa state) đã chuyển về từng tool. `drawGhostPreview` từ 90 dòng còn 13.
- **Strategy không còn bị vòng qua** — chuột phải trước đây gọi hàm **tĩnh** `EraseTool::erase()`, khiến member `eraseTool_` khai báo rồi không dùng bao giờ và dedupe ô mất tác dụng (giữ chuột phải đứng yên vẫn xoá lại mỗi frame → dựng lại canvas 60 lần/giây). Nay đi qua một `EraseTool` thật.
- **`CustomMapData`** thêm `keyOf/gridXOf/gridYOf` (công thức `gy*width+gx` trước đây chép tay 14 chỗ), `setTile/getTile` **có kiểm tra biên** (trước đây `setTile(width,0)` âm thầm ghi đè ô `(0,1)`), và `countBosses()/countEnemies()`.
- **`MapEditorState`** bỏ `EditorTextureCache::unloadAll()` khỏi destructor — singleton sống theo tiến trình, để instance huỷ nó làm instance editor còn nằm dưới stack mất sạch texture.
- Gỡ `EditorMapResizer::process(data, wts)` — overload 2 tham số thân là placeholder `return false`, không ai gọi.

**Enemy trong custom map:** `EntityPalette` nay có 9 mob + 6 boss (tên giữ tiền tố `Mob_`/`Boss_` vì `spawnEntitiesFromMap()` phân nhánh bằng đúng hai tiền tố này). Trước đây palette không có một enemy nào, và kể cả có thì luồng spawn custom cũng chỉ gọi `ItemFactory`.

**Thay đổi kèm theo ở các class có sẵn:**
- `Player::createSaveData()` / `restoreFromSaveData()` — Player tự đóng gói mình, `BaseLevelState` hết phải thò tay vào `getRuntimeStatsMutable()`.
- `BaseLevelState::onCheckpointReached()` — gom logic lưu về một chỗ (trước đây trùng lặp ở 2 nơi với 2 đường dẫn khác nhau).
- `BaseLevelState(mapFilePath, GameSaveData)` — constructor uỷ quyền cho ctor LDtk rồi khôi phục trạng thái.
- `World01State`..`World06State` — thêm ctor `explicit World0XState(const GameSaveData&)`.
- `ItemState` tách khỏi `BaseItem.h` ra `include/entity/Item/ItemState.h` để tầng save không phải kéo theo `Entity.h`/`raylib.h`.

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
