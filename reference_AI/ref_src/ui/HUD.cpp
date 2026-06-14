#include "HUD.h"
#include "raylib.h"

void HUD::Draw(const Player& player) {
    DrawRectangle(8, 8, 260, 110, { 0, 0, 0, 120 });
    DrawText("SUPER MARIO PLUS",   16, 14, 18, YELLOW);
    DrawText(TextFormat("FPS: %d", GetFPS()), 16, 36, 16, WHITE);
    DrawText(TextFormat("X: %.0f  Y: %.0f", player.GetPosition().x, player.GetPosition().y), 16, 56, 16, LIGHTGRAY);
    DrawText("A/D:Move  Space:Jump  S:Crouch", 16, 98, 13, GRAY);
}
