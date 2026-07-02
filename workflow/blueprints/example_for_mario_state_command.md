# Phân tích chi tiết Kiến trúc: Entity, State, Command

Tài liệu này bóc tách riêng rẽ cấu trúc kế thừa và cách hoạt động của từng thành phần, sau đó gộp chúng lại để thấy bức tranh toàn cảnh về cách hệ thống hoạt động.

---

## 1. Thành phần ENTITY (Thực thể)

### Sơ đồ Kế thừa
`Entity` là lớp cơ sở chứa các thuộc tính vật lý (tọa độ, vận tốc) và định nghĩa các hành động cơ bản (jump, move). `Mario` là một thực thể cụ thể kế thừa từ `Entity`.

```mermaid
classDiagram
    class Entity {
        <<abstract>>
        #Vector2 position
        #Vector2 velocity
        +jump()*
        +moveRight()*
        +update(dt)*
    }
    class Mario {
        -IEntityState~Mario~* currentState
        +jump()
        +moveRight()
        +update(dt)
        +changeState()
    }
    Entity <|-- Mario
```

### Cách hoạt động
**Mario** đóng vai trò là vỏ bọc (**Context**). Nó không tự dùng lệnh `if-else` dài dòng để xem "đang ở trên không thì có được nhảy không" hay "đang chạy thì ấn nhảy sẽ thế nào". Thay vào đó, Mario giữ một cái ruột là `currentState`, mỗi khi có lệnh (như `jump()`), Mario sẽ "ủy quyền" (delegate) lệnh đó cho `currentState` xử lý.

---

## 2. Thành phần STATE (Trạng thái)

### Sơ đồ Kế thừa
Tất cả các trạng thái đều tuân theo Interface `IEntityState`. Mỗi trạng thái cụ thể (Idle, Run, Jump) sẽ định nghĩa lại cách thực thể phản ứng với các hành động trong lúc đang ở trạng thái đó.

```mermaid
classDiagram
    class IEntityState~T~ {
        <<interface>>
        +onJump(T& entity)*
        +onMoveRight(T& entity)*
        +update(T& entity, dt)*
        +onEnter(T& entity)*
    }
    class MarioIdleState {
        +onJump(Mario& player)
        +onEnter(Mario& player)
    }
    class MarioRunState {
        +onJump(Mario& player)
        +onStopMove(Mario& player)
    }
    class MarioJumpState {
        +update(Mario& player, dt)
    }

    IEntityState~T~ <|-- MarioIdleState
    IEntityState~T~ <|-- MarioRunState
    IEntityState~T~ <|-- MarioJumpState
```

### Cách hoạt động
State là nơi chứa **logic điều kiện**. Ví dụ: 
- Khi gọi `onJump()` trong `MarioIdleState`, nó cho phép nhảy và yêu cầu Mario chuyển sang `JumpState`. 
- Nhưng trong `MarioJumpState`, hàm `onJump()` có thể không làm gì cả (tránh nhảy đúp).

---

## 3. Thành phần COMMAND (Mệnh lệnh)

### Sơ đồ Kế thừa
Các lệnh từ người chơi (hoặc AI) không được gọi thẳng vào `Mario` mà được đóng gói thành các object `Command` độc lập.

```mermaid
classDiagram
    class IEntityCommand {
        <<interface>>
        +Execute(Entity& entity)*
    }
    class JumpCommand {
        +Execute(Entity& entity)
    }
    class MoveRightCommand {
        +Execute(Entity& entity)
    }

    IEntityCommand <|-- JumpCommand
    IEntityCommand <|-- MoveRightCommand
```

### Cách hoạt động
`InputHandler` sẽ đọc phím bấm và trả về một `Command` tương ứng. 
Lệnh này chỉ có một nhiệm vụ duy nhất: Khi hàm `Execute()` của nó được gọi, nó sẽ gọi đúng hàm tương ứng trên `Entity` truyền vào. (Ví dụ: `JumpCommand` thì gọi `entity.jump()`). Nó không cần biết `Entity` đó là Mario hay Goomba.

---

## 4. GỘP CHUNG: Mối liên hệ và Luồng hoạt động

Khi gộp 3 hệ thống này lại, ta có một kiến trúc rất lỏng lẻo (decoupled) nhưng hoạt động ăn ý theo luồng:
**Người chơi Input -> Tạo Command -> Execute trên Entity -> Entity ủy quyền cho State -> State ra lệnh Entity chuyển trạng thái**.

### Sơ đồ Lớp Tổng Hợp (Mermaid)
Bức tranh toàn cảnh thể hiện sự "lỏng lẻo" trong thiết kế: Mọi thứ liên kết với nhau qua các giao diện (Interfaces).

```mermaid
classDiagram
    direction LR

    %% --- ENTITY ---
    class Entity {
        <<abstract>>
        +jump()
    }
    class Mario {
        -IEntityState~Mario~* currentState
        +jump()
        +setRequest()
    }
    Entity <|-- Mario

    %% --- STATE ---
    class IEntityState~T~ {
        <<interface>>
        +onJump(T& entity)
    }
    class MarioIdleState
    class MarioJumpState
    IEntityState~T~ <|-- MarioIdleState : T=Mario
    IEntityState~T~ <|-- MarioJumpState : T=Mario

    %% --- COMMAND ---
    class IEntityCommand {
        <<interface>>
        +Execute(Entity& entity)
    }
    class JumpCommand
    IEntityCommand <|-- JumpCommand

    %% --- LIÊN KẾT ---
    Mario *-- IEntityState~T~ : Sở hữu
    IEntityCommand ..> Entity : Tác động lên
    MarioIdleState ..> Mario : Gọi lại setRequest
```

### Sơ đồ Luồng Tương tác Tổng Hợp (Mermaid)
Sơ đồ mô phỏng vòng lặp chuẩn khi người chơi nhấn nút Nhảy lúc đang đứng yên. Bạn có thể thấy rõ dòng đời của một lệnh truyền từ Input vào tận sâu bên trong State như thế nào.

```mermaid
sequenceDiagram
    autonumber
    actor Input as InputHandler
    participant Cmd as JumpCommand
    participant Mario as Mario (Entity)
    participant State as MarioIdleState (State)

    Input->>Cmd: Tạo JumpCommand
    activate Input
    Input->>Cmd: Execute(Mario)
    activate Cmd

    Cmd->>Mario: Mario.jump()
    activate Mario

    Mario->>State: currentState->onJump(*this)
    activate State
    Note right of State: State đang là Idle.<br/>Việc nhảy là hợp lệ.

    State->>Mario: setRequest(JumpState)
    Note right of Mario: Ghi nhận yêu cầu<br/>chưa đổi State ngay.
    deactivate State

    Mario-->>Cmd: trả về
    deactivate Mario
    Cmd-->>Input: trả về
    deactivate Cmd
    deactivate Input

    Note over Mario: Lát sau, trong hàm update() của game
    Mario->>Mario: processRequest()
    Note right of Mario: Chính thức gán:<br/>currentState = JumpState
```
