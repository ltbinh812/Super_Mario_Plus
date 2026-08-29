#pragma once
#include "DialogueData.h"
#include "raylib.h"
#include <string>

/**
 * DialogueBox — Class chịu trách nhiệm render hộp thoại RPG lên màn hình.
 *
 * Tuân thủ 4 giai đoạn game loop:
 *   handleInput() → process() → update(dt) → render(alpha)
 *
 * Tính năng:
 *   - Typewriter effect (text chạy từng ký tự)
 *   - Skip (nhấn phím → hiện toàn bộ text)
 *   - Auto-advance indicator ("▼" nhấp nháy khi text xong)
 *   - Portrait hiển thị bên trái (nếu có)
 *   - Tên nhân vật hiển thị phía trên text
 *
 * OOP: Đóng gói (Encapsulation)
 *   - Gom toàn bộ logic typewriter, rendering, input vào 1 class
 *   - Bên ngoài chỉ cần gọi start() → handleInput/process/update/render → isFinished()
 */
class DialogueBox {
private:
    const DialogueSequence* currentSequence = nullptr;
    int currentLineIndex = 0;     // Dòng hội thoại hiện tại
    float charTimer = 0.0f;       // Timer cho typewriter effect
    int visibleChars = 0;         // Số ký tự đang hiển thị
    bool lineComplete = false;    // Dòng hiện tại đã hiển thị hết chưa
    bool waitingForInput = false; // Đang chờ player nhấn phím để tiếp tục
    bool finished = false;        // Toàn bộ sequence đã xong
    float indicatorTimer = 0.0f;  // Timer cho nhấp nháy "▼"
    bool active = false;

    // Layout constants (screen space, pixels)
    static constexpr float BOX_MARGIN_X = 40.0f;
    static constexpr float BOX_MARGIN_BOTTOM = 20.0f;
    static constexpr float BOX_HEIGHT = 120.0f;
    static constexpr float BOX_PADDING = 16.0f;
    static constexpr float PORTRAIT_SIZE = 80.0f;
    static constexpr float NAME_FONT_SIZE = 20.0f;
    static constexpr float TEXT_FONT_SIZE = 16.0f;
    static constexpr float INDICATOR_BLINK_SPEED = 3.0f; // Hz

    // Lấy text hiện tại đã cắt theo visibleChars (xử lý UTF-8 an toàn)
    std::string getVisibleText() const;

    // Tính số ký tự (codepoints) trong chuỗi UTF-8
    int getTextLength(const std::string& text) const;

public:
    DialogueBox() = default;

    /**
     * Bắt đầu hiển thị dialogue sequence.
     * @param sequence Pointer đến DialogueSequence (phải tồn tại suốt thời gian box active)
     */
    void start(const DialogueSequence* sequence);

    /**
     * Dừng và reset trạng thái.
     */
    void stop();

    // === 4 giai đoạn game loop ===
    void handleInput();           // Đọc phím Enter/Space
    void process();               // Tính toán trạng thái
    void update(float dt);        // Update typewriter timer
    void render(float alpha) const; // Vẽ box lên screen space

    bool isFinished() const { return finished; }
    bool isActive() const { return active; }
};
