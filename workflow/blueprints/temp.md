# Combat and Attack System Architecture Plan

Chào bạn, dựa trên source code hiện tại, đây là bản thiết kế cho hệ thống Attack/Combat theo chuẩn OOP và Design Patterns. 

## 1. Thiết kế theo Interface Segregation (IDamageable)
Hiện tại `Entity` đang chứa mọi thứ. Để clean code và tuân thủ nguyên lý SOLID, chúng ta nên tách khả năng "nhận sát thương" ra một interface riêng.

```cpp
struct DamageInfo {
    int amount;
    Vector2 knockbackForce;
    Entity* source; // Kẻ ra đòn
};

class IDamageable {
public:
    virtual ~IDamageable() = default;
    virtual void takeDamage(const DamageInfo& info) = 0;
};
```
**Lý do:** Không phải Entity nào cũng có máu và nhận sát thương (ví dụ: các bục nhảy, item). Chỉ những Entity nào kế thừa `IDamageable` mới bị ảnh hưởng bởi Attack. `Player` và `Enemy` sẽ kế thừa interface này.

## 2. Tối ưu hóa việc kiểm tra va chạm đòn đánh (Spatial Partitioning Pattern)
Làm sao để biết ai bị đánh trúng? Duyệt mảng O(N) mọi quái vật trong game mỗi frame là cách không tối ưu nếu map lớn và nhiều quái.
> [!TIP]
> Thay vì check toàn bộ quái, chúng ta áp dụng **Spatial Partitioning Pattern (Phân vùng không gian)**, cụ thể là cấu trúc **Grid / Cell** hoặc **QuadTree**.

* **Cách hoạt động (Grid-based):** 
  Chia bản đồ thành các ô lưới (ví dụ mỗi ô 100x100 pixel). Mỗi Entity sẽ được đăng ký vào ô mà nó đang đứng.
* **Tối ưu:** Khi Player tung đòn đánh (Attack Hitbox xuất hiện), ta chỉ cần lấy tọa độ của Hitbox đó, tính ra nó đang nằm ở ô nào, và **chỉ kiểm tra va chạm với các Entity nằm trong ô đó (và các ô lân cận sát vách)**.
* Từ O(N) với N là hàng trăm quái vật, số lượng kiểm tra sẽ giảm xuống chỉ còn O(k) với k là vài con quái ở gần người chơi.

## 3. Xử lý các dạng sát thương khác như Bắn Đạn (Object Pool & Strategy Pattern)
Đối với đòn đánh xa (Ranged Attack / Đạn / Cầu lửa), viên đạn bản chất cũng là một `Entity` có Hitbox, mang theo `DamageInfo` và di chuyển mỗi frame. Khi đạn va chạm với `IDamageable`, nó gọi `takeDamage` rồi biến mất. 
Để quản lý hệ thống này chuẩn OOP và tối ưu hiệu năng, ta dùng các pattern sau:

### a. Object Pool Pattern (Bắt buộc cho đạn)
Đạn được bắn ra và biến mất liên tục. Việc gọi `new Bullet()` và `delete bullet` liên tục sẽ làm phân mảnh bộ nhớ (Memory Fragmentation) và tụt FPS nghiêm trọng (Garbage collection spike / Cache miss).
* **Giải pháp:** Tạo một `BulletPool` chứa sẵn một mảng cố định (ví dụ 100 viên đạn) từ khi khởi động màn chơi. 
* Các viên đạn ban đầu có trạng thái `isActive = false`. 
* Khi người chơi bắn đạn, `BulletPool` tìm viên đạn đầu tiên đang `false`, kích hoạt thành `true`, reset lại tọa độ ngay nòng súng và vận tốc bay.
* Khi đạn trúng đích hoặc bay ra ngoài màn hình, set `isActive = false` trả nó về Pool mà không giải phóng bộ nhớ.

### b. Factory Method Pattern (Tạo nhiều loại đạn)
Nếu game có nhiều loại súng/phép (đạn lửa, đạn băng, tên lửa đuổi), dùng **Factory** để instantiate và gán thông số từ Pool cho phù hợp mà không cần if-else dài ngoằng trong code Player.

### c. Strategy Pattern (Hành vi của đạn)
Nếu quỹ đạo đạn khác nhau: 
* `StraightFlightStrategy`: Bay đường thẳng.
* `SineWaveFlightStrategy`: Bay hình sin.
* `HomingFlightStrategy`: Tự động đuổi theo quái gần nhất.
Mỗi viên đạn (Projectile Entity) sẽ chứa một con trỏ `IMovementStrategy*` để tự động tính toán vị trí mới mỗi frame.

## 4. Quá trình xử lý sát thương khi trúng đòn
Khi `target->takeDamage(info)` được gọi (do trúng đòn cận chiến hoặc bị đạn bay trúng), Entity bị đánh sẽ tự xử lý:

```cpp
void Player::takeDamage(const DamageInfo& info) override {
    // Strategy Pattern: Có thể tiêm (inject) công thức tính Damage vào đây
    int actualDamage = std::max(1, info.amount - this->runtimeStats.defense);
    this->runtimeStats.health -= actualDamage;

    if (this->runtimeStats.health <= 0) {
        changeState(dieState);
    } else {
        // Áp dụng knockback
        this->runtimeStats.velocity = info.knockbackForce;
        changeState(hurtState); // HurtState sẽ cung cấp i-frames (vô địch tạm thời)
    }
}
```

## 5. Quản lý Quái vật trên Map siêu to (Spawning & Culling)
Với một bản đồ rất lớn (ví dụ map thế giới mở hoặc map đi cảnh dài), việc khởi tạo (load) và update TẤT CẢ quái vật trên map cùng một lúc là **tối kỵ** vì sẽ gây quá tải CPU và tốn RAM. 
Để giải quyết, chúng ta sử dụng kết hợp **Spawner Pattern** và **Camera Culling** (kỹ thuật Chunking/Sectoring).

### a. Không lưu Entity, chỉ lưu dữ liệu thô (Spawn Points)
Khi parse file Map (từ Tiled hoặc JSON), ta không gọi `new Enemy()` ngay. Ta chỉ lưu tọa độ và loại quái vật vào một mảng chứa dữ liệu thô (gọi là `SpawnPoint` hoặc `EntityDef`). 
```cpp
struct SpawnPoint {
    Vector2 position;
    EnemyType type;
    bool isSpawned = false; // Đã được tạo ra chưa?
};
std::vector<SpawnPoint> levelSpawnPoints;
```

### b. Kỹ thuật Culling dựa trên Camera (Active Window)
* Ở mỗi frame (hoặc check định kỳ mỗi nửa giây để tối ưu hơn), ta lấy khung nhìn của Camera (Viewport) và mở rộng ra một chút (ví dụ cộng thêm 200-300 pixel mỗi chiều) để làm "Vùng hoạt động" (Active Zone).
* **Quá trình Spawn:** Duyệt qua danh sách `levelSpawnPoints`. Nếu điểm spawn nằm TRONG "Vùng hoạt động" và `isSpawned == false`, ta mới thực sự lấy một `Enemy` từ trong **Object Pool** (tương tự như Pool của đạn), khởi tạo nó tại vị trí đó, và đánh dấu `isSpawned = true`.
* **Quá trình Despawn (Culling):** Khi duyệt update các `Enemy` đang sống, nếu kiểm tra thấy một `Enemy` di chuyển ra KHỎI "Vùng hoạt động" quá xa, ta lập tức thu hồi nó về Object Pool (trạng thái `isActive = false`) để tiết kiệm tài nguyên. Cùng lúc đó, cập nhật lại trạng thái tương ứng trong `levelSpawnPoints` là `isSpawned = false` (để lỡ người chơi quay lại thì quái lại hiện ra).

> [!TIP]
> Kỹ thuật này đảm bảo dù map của bạn dài tới đâu, số lượng quái vật thực sự tồn tại trong RAM và cần check va chạm luôn bị giới hạn (chỉ tầm 10-20 con nằm ngay sát màn hình người chơi).

### c. Xử lý Culling cho Game Nhiều Người Chơi (Dynamic Zoom Camera)
Trong các game Co-op có nhiều Player, Camera thường tự động zoom out (thu nhỏ hình ảnh) khi 2 người chơi đứng cách xa nhau để giữ cả hai trên màn hình.
* **Vấn đề nếu dùng Camera:** Nếu ta gán "Vùng hoạt động" (Active Zone) trực tiếp vào Viewport của Camera, thì khi Camera zoom out rộng ra, Active Zone sẽ phình to khổng lồ -> Đột ngột Load một lượng quái vật khổng lồ vào RAM -> Game bị giật lag (Lag spike). Ngoài ra, logic của game không nên phụ thuộc vào yếu tố render/đồ họa như độ zoom của camera.
* **Cách giải quyết (Player-Centric Zone):** Đừng xét vùng hoạt động quanh Camera, hãy xét quanh người chơi!
  1. Mỗi frame, tìm tọa độ `minX`, `minY`, `maxX`, `maxY` bao bọc tất cả các Player đang sống.
  2. Tạo ra một Bounding Box (Hình chữ nhật) từ các tọa độ trên.
  3. Cộng thêm một khoảng đệm cố định (Margin / Padding) xung quanh Box đó (ví dụ mỗi cạnh cộng thêm 800 pixel tương đương 1 màn hình).
  4. Lấy Box vừa tạo làm "Vùng hoạt động" để check Spawn/Culling.

> [!TIP]
> Bằng cách lấy Player làm gốc tọa độ cho Active Zone, bạn vừa tách biệt hoàn toàn logic game khỏi hệ thống đồ họa, vừa đảm bảo tính ổn định hiệu năng dù Camera có thay đổi hay zoom out đến mức nào.


### d. Xử lý Quái vật đã spawn nhưng lọt ra ngoài Vùng hoạt động (Despawning & Hibernation)
Khi một con quái vật đang hoạt động (ví dụ đang rượt đuổi người chơi) nhưng sau đó bị lọt ra ngoài "Vùng hoạt động" (do người chơi chạy đi quá xa), chúng ta có 3 hướng xử lý chuẩn tùy theo thể loại game:

1. **Hard Reset (Kiểu Mario / Contra cổ điển):** 
   - Lập tức thu hồi quái vật về Object Pool (`isActive = false`). 
   - Đặt lại biến ở điểm spawn của nó thành `isSpawned = false`. 
   - **Hệ quả:** Khi người chơi quay lại điểm đó, quái vật sẽ hồi sinh đầy máu ở đúng vị trí gốc ban đầu. Cách này dễ code và giải phóng tài nguyên triệt để nhất.
2. **Persistence - Lưu trạng thái (Kiểu Metroidvania):** 
   - Cũng thu hồi quái vật về Object Pool, nhưng trước khi thu hồi, ta *lưu lại lượng máu hiện tại và tọa độ hiện tại* của nó ghi đè lên dữ liệu của điểm `SpawnPoint` ban đầu.
   - **Hệ quả:** Người chơi quay lại sẽ thấy con quái vẫn đang ở trạng thái dở dang (còn nửa cây máu và đứng ở tọa độ mà nó bị bỏ lại).
3. **Hibernation - Ngủ đông (Kiểu RPG / Open World):**
   - Quái vật KHÔNG bị thu hồi về Object Pool, nó vẫn nằm nguyên trong danh sách Active Entities của RAM.
   - Tuy nhiên, ở đầu hàm `update()`, ta chặn lại bằng một dòng check: `if (!isInActiveZone) return;`. 
   - **Hệ quả:** Con quái bị "đóng băng", nó không thèm tính toán AI, không di chuyển, không check vật lý để giải phóng CPU. Nhưng khi lọt lại vào Vùng hoạt động, nó lập tức tiếp tục hành động ngay lập tức mà không tốn công Spawn lại. Dùng cách này nếu game của bạn không có quá nhiều quái hoặc đây là một con Boss/Mini-boss không được phép biến mất.

    -> tui nghĩ mình sẽ xài cách 3 á

> [!TIP]
> **Khuyến nghị:** Đối với game đi cảnh (Platformer) sử dụng Map siêu to, hãy dùng cách 1 (Hard Reset) cho lính lác thông thường để tối ưu cả RAM và CPU. Dùng cách 3 (Hibernation) dành riêng cho Boss.

## Open Questions
> [!IMPORTANT]
> 1. Hiện tại Project của bạn đã có hệ thống quản lý mảng/list các Entity (`EntityManager` hoặc vòng lặp chung) chưa? Nếu có, nó đang lưu trữ dưới dạng std::vector hay dạng nào?
> 2. Về việc di chuyển đạn, bạn muốn đạn chịu ảnh hưởng của trọng lực (như ném lựu đạn) hay bay thẳng bất chấp trọng lực? Đạn bay thẳng thì không cần gọi hàm `updatePhysicsWithMap`, giúp tiết kiệm thêm hiệu năng.
> 3. Bạn dự định dùng hệ thống Map hiện tại (TileMap) hay parse từ tool bên ngoài (như Tiled)? Việc đặt `SpawnPoint` cho quái nên được đọc từ một layer riêng trên file cấu trúc Map.
