#include "DialogueBox.h"
#include "AssetManager.h"
#include <iostream>
#include <cmath>

// === UTF-8 helper: đếm số codepoint trong chuỗi UTF-8 ===
int DialogueBox::getTextLength(const std::string& text) const {
    int count = 0;
    for (size_t i = 0; i < text.size(); ) {
        unsigned char c = text[i];
        if (c < 0x80)       i += 1; // ASCII
        else if (c < 0xE0)  i += 2; // 2-byte
        else if (c < 0xF0)  i += 3; // 3-byte (tiếng Việt rơi vào đây)
        else                i += 4; // 4-byte (emoji, etc.)
        count++;
    }
    return count;
}

// === UTF-8 helper: lấy chuỗi con gồm n codepoint đầu tiên ===
std::string DialogueBox::getVisibleText() const {
    if (!currentSequence || currentLineIndex >= (int)currentSequence->lines.size())
        return "";

    const std::string& fullText = currentSequence->lines[currentLineIndex].text;
    int count = 0;
    size_t bytePos = 0;

    while (bytePos < fullText.size() && count < visibleChars) {
        unsigned char c = fullText[bytePos];
        if (c < 0x80)       bytePos += 1;
        else if (c < 0xE0)  bytePos += 2;
        else if (c < 0xF0)  bytePos += 3;
        else                bytePos += 4;
        count++;
    }

    return fullText.substr(0, bytePos);
}

void DialogueBox::start(const DialogueSequence* sequence) {
    if (!sequence || sequence->lines.empty()) {
        std::cerr << "[DialogueBox] Cannot start: null or empty sequence\n";
        return;
    }

    currentSequence = sequence;
    currentLineIndex = 0;
    charTimer = 0.0f;
    visibleChars = 0;
    lineComplete = false;
    waitingForInput = false;
    finished = false;
    indicatorTimer = 0.0f;
    active = true;
}

void DialogueBox::stop() {
    currentSequence = nullptr;
    active = false;
    finished = true;
    StopSound(AssetManager::getInstance().getSound("typewriter_sound"));
}

// === handleInput(): Chỉ đọc phím, không thay đổi trạng thái ===
void DialogueBox::handleInput() {
    if (!active || finished) return;

    // Nhận input từ phím Enter hoặc Space
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (!lineComplete) {
            // Typewriter đang chạy → skip, hiện hết text
            lineComplete = true;
            if (currentSequence) {
                visibleChars = getTextLength(currentSequence->lines[currentLineIndex].text);
            }
            StopSound(AssetManager::getInstance().getSound("typewriter_sound"));
        } else {
            // Text đã hiện hết → đánh dấu chờ chuyển dòng
            waitingForInput = true;
        }
    }
}

// === process(): Kiểm tra trạng thái, quyết định chuyển dòng ===
void DialogueBox::process() {
    if (!active || finished || !currentSequence) return;

    if (waitingForInput) {
        waitingForInput = false;
        currentLineIndex++;

        if (currentLineIndex >= (int)currentSequence->lines.size()) {
            // Hết tất cả dòng → kết thúc
            finished = true;
            active = false;
        } else {
            // Chuyển sang dòng tiếp theo
            charTimer = 0.0f;
            visibleChars = 0;
            lineComplete = false;
        }
    }
}

// === update(dt): Cập nhật typewriter timer và indicator blink ===
void DialogueBox::update(float dt) {
    if (!active || finished || !currentSequence) return;

    if (!lineComplete) {
        const DialogueLine& line = currentSequence->lines[currentLineIndex];
        int totalChars = getTextLength(line.text);

        charTimer += dt;
        float charInterval = (line.textSpeed > 0.0f) ? (1.0f / line.textSpeed) : 0.0f;

        // Tăng số ký tự hiển thị theo thời gian
        bool characterRevealed = false;
        while (charTimer >= charInterval && visibleChars < totalChars) {
            charTimer -= charInterval;
            visibleChars++;
            characterRevealed = true;
        }

        if (characterRevealed) {
            PlaySound(AssetManager::getInstance().getSound("typewriter_sound"));
        }

        if (visibleChars >= totalChars) {
            lineComplete = true;
            visibleChars = totalChars;
            StopSound(AssetManager::getInstance().getSound("typewriter_sound"));
        }
    }

    // Timer cho indicator nhấp nháy "▼"
    indicatorTimer += dt;
}

// === render(): Chỉ vẽ, không thay đổi state ===
void DialogueBox::render(float alpha) const {
    if (!active || !currentSequence || currentLineIndex >= (int)currentSequence->lines.size())
        return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // === Vị trí box (dưới cùng màn hình) ===
    float boxX = BOX_MARGIN_X;
    float boxY = screenH - BOX_HEIGHT - BOX_MARGIN_BOTTOM;
    float boxW = screenW - BOX_MARGIN_X * 2.0f;

    // === Vẽ backdrop (semi-transparent) ===
    DrawRectangle((int)boxX, (int)boxY, (int)boxW, (int)BOX_HEIGHT,
                  Color{0, 0, 0, 200});
    // Border
    DrawRectangleLines((int)boxX, (int)boxY, (int)boxW, (int)BOX_HEIGHT, WHITE);

    const DialogueLine& line = currentSequence->lines[currentLineIndex];

    // === Content offset (bên trong box, sau portrait nếu có) ===
    float contentX = boxX + BOX_PADDING;
    float contentY = boxY + BOX_PADDING;

    // === Vẽ portrait (nếu có) ===
    bool hasPortrait = !line.portraitKey.empty();
    if (hasPortrait) {
        // Thử load portrait texture từ AssetManager
        try {
            const Texture2D& portrait = AssetManager::getInstance().getTexture(line.portraitKey);
            Rectangle src = {0, 0, (float)portrait.width, (float)portrait.height};
            Rectangle dst = {contentX, contentY, PORTRAIT_SIZE, PORTRAIT_SIZE};
            DrawTexturePro(portrait, src, dst, {0, 0}, 0.0f, WHITE);
            contentX += PORTRAIT_SIZE + BOX_PADDING;
        } catch (...) {
            // Portrait không tìm thấy → skip, vẽ text bình thường
            hasPortrait = false;
        }
    }

    // === Vẽ tên nhân vật ===
    if (!line.speakerName.empty()) {
        DrawText(line.speakerName.c_str(), (int)contentX, (int)contentY,
                 (int)NAME_FONT_SIZE, GOLD);
        contentY += NAME_FONT_SIZE + 4.0f;
    }

    // === Vẽ text (typewriter) ===
    std::string visible = getVisibleText();
    if (!visible.empty()) {
        // Word wrap đơn giản: để text tự xuống dòng trong khu vực box
        float maxTextW = boxX + boxW - BOX_PADDING - contentX;
        
        // Vẽ từng dòng, tách text theo chiều ngang
        DrawText(visible.c_str(), (int)contentX, (int)contentY,
                 (int)TEXT_FONT_SIZE, WHITE);
    }

    // === Vẽ indicator "▼" nhấp nháy khi text xong ===
    if (lineComplete) {
        float blinkAlpha = (std::sin(indicatorTimer * INDICATOR_BLINK_SPEED * 2.0f * 3.14159f) + 1.0f) / 2.0f;
        unsigned char alpha8 = (unsigned char)(blinkAlpha * 255.0f);
        float indX = boxX + boxW - BOX_PADDING - 16.0f;
        float indY = boxY + BOX_HEIGHT - BOX_PADDING - 16.0f;
        DrawText("\x76", (int)indX, (int)indY, 16, Color{255, 255, 255, alpha8}); // "v" as indicator
    }
}
