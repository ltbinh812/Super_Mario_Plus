## 1. Giai đoạn Khởi tạo (Initialization Phase)
Mọi thứ bắt đầu từ class Game – "nhạc trưởng" điều phối toàn bộ hệ thống.

Khi ứng dụng chạy, hàm Game::init() sẽ được gọi.

Nó khởi tạo Window để thiết lập cửa sổ game (thực chất là bọc các hàm của Raylib như InitWindow()).

Đồng thời, nó cũng tạo ra và kết nối SceneManager (quản lý các màn hình game) với GameLoop (quản lý thời gian và chu kỳ update).

## 2. Vòng lặp Game (The Game Loop)
Sau khi khởi tạo xong, Game::run() sẽ kích hoạt GameLoop::run(). Đây là trái tim của game, chạy liên tục cho đến khi người dùng thoát.

Fixed-Timestep (Bước thời gian cố định): Để đảm bảo game chạy ổn định trên mọi máy tính dù máy mạnh hay yếu, GameLoop dùng biến accumulator để gom thời gian. Logic vật lý và va chạm luôn được cập nhật bằng một lượng dt cố định.

Nội suy đồ họa (Interpolation): Nó sinh ra một giá trị alpha truyền xuống hàm render. Việc này giúp hình ảnh chuyển động cực kỳ mượt mà giữa các frame vật lý.

Trong mỗi vòng lặp, GameLoop chỉ làm nhiệm vụ gọi SceneManager.update(dt) và sau đó là SceneManager.render(alpha).

## 3. Quản lý Trạng thái (Scene & State Management)
Đây là phần thú vị nhất của kiến trúc này. Khi GameLoop yêu cầu update/render, SceneManager sẽ quyết định xem cái gì đang được thực thi dựa trên cấu trúc dữ liệu Stack (Ngăn xếp).

Mọi màn hình (Logo, Menu, Đang chơi, Game Over) đều là một lớp kế thừa từ interface IGameState.

Luồng cơ bản: Bắt đầu game, SceneManager sẽ push SplashState (màn hình Logo) vào Stack. Chạy xong logo, nó pop (xóa) trạng thái đó ra và push MenuState vào.

Sức mạnh của Stack (Overlays): Giả sử bạn đang chơi (PlayState) và nhấn nút Pause. Thay vì xóa PlayState đi, SceneManager chỉ việc push thêm PauseState đè lên trên cùng của Stack.

Lúc này, hàm update() chỉ chạy cho PauseState ở trên cùng (nên game dừng lại, quái vật không di chuyển).

Nhưng hàm render() có thể được code để vẽ cả PlayState (làm nền mờ ở dưới) và PauseState đè lên trên.

Khi chọn "Resume", hệ thống chỉ cần pop PauseState ra, PlayState lại trồi lên trên cùng và game tiếp tục chính xác từ vị trí đã dừng.

## 4. Lớp Giao diện (UI Layer)
Các State sẽ gọi đến UI Layer để hiển thị thông tin. Kiến trúc này làm rất tốt việc Decoupling (Giảm thiểu phụ thuộc):

UI components (HUD, Menu) hoàn toàn stateless (không lưu trữ trạng thái của game). Ví dụ: HUD không chứa biến lưu số mạng (lives) hay điểm số (score).

Thay vào đó, ở mỗi frame, PlayState sẽ lấy data từ nhân vật và truyền thẳng vào hàm HUD::render(score, coins, world, lives, timer).

Do Raylib sử dụng Global Graphics Context (ngữ cảnh đồ họa toàn cục), các class này không cần phải truyền object Renderer lằng nhằng qua từng hàm. NumberRenderer hay HUD có thể gọi trực tiếp các lệnh vẽ ra màn hình.

Tóm tắt vòng đời của 1 Frame hiển thị:
1. Window::beginDraw(): Dọn dẹp màn hình cũ (ClearScreen).

2. GameLoop: Tính toán lượng thời gian trôi qua (dt và alpha).

3. GameLoop -> SceneManager: Yêu cầu cập nhật logic. SceneManager gọi hàm update() của State nằm trên cùng Stack.

4. GameLoop -> SceneManager: Yêu cầu vẽ đồ họa. State hiện tại sẽ vẽ nhân vật, môi trường, sau đó gọi tiếp HUD hoặc Menu để vẽ giao diện.

5. Window::endDraw(): Đóng bộ đệm đồ họa và đẩy khung hình mới tinh lên màn hình người chơi.




--- 

# CÁC SCENE/UI CHÍNH 

## 1. Tương tác với World Layer (Môi trường & Camera)
PlayState sẽ là người "sở hữu" (owner) bản đồ và góc nhìn của màn chơi hiện tại.

Lúc bắt đầu (onEnter): Nó sẽ gọi LevelLoader::loadLevel(...) để đọc file JSON. Quá trình này sẽ sinh ra object TileMap và danh sách các con quái (Entities).

Trong vòng lặp (update): Gọi Camera.update() để camera bám theo tọa độ hiện tại của nhân vật Mario.

Lúc vẽ (render): Gọi TileMap.render(renderer, camera). Nó truyền camera vào để World tự biết culling (chỉ vẽ những block đang hiển thị trên màn hình).

## 2. Tương tác với Entity Layer (Dữ liệu game)
State quản lý toàn bộ vòng đời của thực thể.

Quản lý danh sách: PlayState sẽ cầm một mảng std::vector<Entity*> activeEntities và một con trỏ riêng Mario* player.

Cập nhật: Lặp qua mảng này và gọi hàm update(dt) của từng entity để quái tự chạy AI của nó.

Đổ dữ liệu ra UI (HUD): Đây là mấu chốt của câu hỏi trước đó. PlayState lấy data từ domain (ví dụ: player->score, player->coins, player->lives) rồi truyền các con số trần trụi này vào hàm HUD.render(score, coins...). UI hoàn toàn không biết class Mario là gì.

## 3. Tương tác với Systems Layer (Hệ thống tính toán)
State là người "cấp phép" cho hệ thống chạy.

PhysicsSystem hay CollisionSystem không tự nhiên chạy được. Trong hàm PlayState::update(), nó sẽ gọi physicsSystem.update(activeEntities, dt).

Tức là State quăng nguyên cái mảng thực thể vào bộ máy nghiền (Systems). Bộ máy này tính toán xong (rơi tự do, giới hạn vận tốc, trừ máu...) thì mảng entities tự động được cập nhật trạng thái mới.

## 4. Tương tác với Infrastructure Layer (Hệ thống nền tảng)
State sử dụng hạ tầng như các công cụ tĩnh (thường qua Singleton pattern) để vận hành các hiệu ứng bề nổi:

Audio: Khi PlayState khởi tạo, nó gọi AudioManager::getInstance().playBGM("overworld_theme"). Khi thời gian còn dưới 100s, nó check điều kiện và gọi playHurryBGM().

EventBus: State vừa là người lắng nghe (Listener) vừa là người phát tín hiệu (Publisher).

Ví dụ: Khi hàm Mario.die() được kích hoạt, nó có thể bắn ra một event qua EventBus. PlayState đang lắng nghe event này, nhận được tin báo thì lập tức kích hoạt hàm SceneManager.changeState(new GameOverState()).

Object Pool & Spatial Hash: State sẽ clear lưới không gian (Spatial Hash) mỗi frame, sau đó nhét lại các entity vào lưới để tối ưu hóa việc check va chạm. Đồng thời, mỗi khi Mario bắn ra một Fireball, State sẽ móc nó ra từ ObjectPool thay vì dùng new để tránh rò rỉ bộ nhớ.

## Tóm tắt lại Flow của một frame trong PlayState sẽ trông như thế này:
Nhận Input (bàn phím).
Xóa SpatialHash, đẩy toàn bộ activeEntities vào lại lưới để chuẩn bị check va chạm.
Chạy PhysicsSystem (kéo mọi thứ rớt xuống đất).
Cập nhật AI của từng Entity (Mario chạy, Goomba đi dạo).
Kích hoạt EventBus::dispatch() (xử lý các sự kiện chết, ăn nấm vừa xảy ra).
Tính toán lại tọa độ Camera cho hợp lý.
Vẽ TileMap (truyền camera vào).
Vẽ từng Entity.
Lấy data từ Mario, truyền vào HUD để vẽ giao diện chèn lên trên cùng.

# PHASE 1: Core Engine (Tuần 1-2)
Giai đoạn này dễ đụng code nhất vì ai cũng cần đụng vào Core. Hãy chia theo bề ngang.
## Dev A (Hạ tầng & Hệ thống): * Setup CMake, cấu trúc thư mục (1.01, 1.02, 1.03).
Class Window, GameLoop, Logger (1.04, 1.05, 1.15).
State Machine cơ bản (1.17, 1.18).
## Dev B (Gameplay & Vật lý): * Class Entity base (1.06).
PhysicsSystem, CollisionSystem cơ bản (1.08, 1.13).
Class Mario cục gạch (di chuyển, nhảy) (1.10, 1.11, 1.14).
TileMap mảng 2D thô sơ (1.12).
## Điểm hội tụ (Cuối tuần 2): 
Dev A tích hợp Mario và TileMap của Dev B vào PlayState chạy vòng lặp của Dev A.

# PHASE 2: World & First Enemy (Tuần 3-4)
Lúc này kiến trúc đã ổn định, có thể chia theo Feature dọc.
## Dev A (Quản lý Tài nguyên & Môi trường):
Chuyển đổi sang Raylib (nếu chưa làm), load Texture/Audio (2.01).
LevelLoader đọc JSON, Camera culling (2.04, 2.05, 2.06).
AudioManager phát nhạc nền, hiệu ứng (2.15, 2.16, 2.17).
## Dev B (Thực thể & Tương tác):
Gắn Sprite/Animation cho Mario (2.02, 2.03).
MarioFSM (Nấm lớn, nhỏ) (2.08, 2.12).
Khối gạch QuestionBlock sinh Item, AI con Goomba (2.09, 2.10, 2.11).
Cùng làm: EventBus và Hệ thống UI (HUD, ScoreSystem).

# PHASE 3: Polish - "Game Feel" (Tuần 5-6)
Giai đoạn tối ưu cảm giác chơi, cần test chéo (A code, B test).
## Dev A (Hệ thống hạt & Tương tác môi trường):
Breakable bricks (hiệu ứng nát gạch) (3.01).
ParticleSystem (bụi, điểm số nảy lên) (3.12, 3.14).
Ráp map 1-1, 1-2, 1-3, làm cơ chế chui ống Pipe (3.09, 3.16).
## Dev B (Vũ khí & Kẻ địch phức tạp):
KoopaTroopa (mai rùa trượt) (3.02, 3.03).
FireFlower, đạn Fireball nảy (3.04, 3.05, 3.06).
Ngôi sao StarPower (3.07, 3.18).
Chỉnh sửa mượt mà cho CollisionSystem (corner correction) (3.15).

# PHASE 4: Advanced (Tuần 7-8)
Đây là lúc team dễ bị đuối nhất, cần ưu tiên các tính năng cốt lõi trước.
## Dev A (State, Data & Tối ưu hóa):
Hoàn thiện UI các màn hình MenuState, PauseState, GameOverState (4.01 - 4.04).
Hệ thống Save/Load JSON và Highscore (4.18, 4.19).
Viết ObjectPool và SpatialHash để tối ưu (đây là lúc kiến thức cấu trúc dữ liệu phát huy tác dụng) (4.20, 4.21).
## Dev B (Boss & Level Design):
Cày AI cho các con quái khó: HammerBro, Lakitu, CheepCheep, boss Bowser (4.05 - 4.11).
Thiết kế màn chơi (Map Editor hoặc tự build JSON) (4.12 - 4.15).

# PHASE 5: Release (Tuần 9)
Mọi người cùng "dọn dẹp".
## Dev A: 
Lo phần build CMake Release, GTest, viết README (5.01, 5.02, 5.04, 5.06).
## Dev B: 
Doxygen comment, review code xóa magic numbers, quay video demo (5.03, 5.05, 5.07, 5.08).