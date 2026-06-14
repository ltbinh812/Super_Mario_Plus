#include "Background.h"

void Background::DrawMountain(Vector2 pos, float w, float h, Color col) {
    DrawTriangle({ pos.x, pos.y - h }, { pos.x - w * 0.5f, pos.y }, { pos.x + w * 0.5f, pos.y }, col);
    float capH = h * 0.22f, capW = w * 0.20f;
    DrawTriangle({ pos.x, pos.y - h }, { pos.x - capW, pos.y - h + capH * 2.0f }, { pos.x + capW, pos.y - h + capH * 2.0f }, { 240, 248, 255, 255 });
}

void Background::Draw(float worldWidth, float groundY, const std::vector<Cloud>& clouds) {
    for (int i = 0; i < 16; i++) {
        float t   = (float)i / 15.0f;
        Color top = { 70,  140, 220, 255 };
        Color bot = { 140, 210, 255, 255 };
        Color col = {
            (unsigned char)(top.r + t * (bot.r - top.r)),
            (unsigned char)(top.g + t * (bot.g - top.g)),
            (unsigned char)(top.b + t * (bot.b - top.b)),
            255
        };
        float y0 = (float)i       / 16.0f * groundY;
        float y1 = (float)(i + 1) / 16.0f * groundY;
        DrawRectangle(0, (int)y0, (int)worldWidth, (int)(y1 - y0) + 1, col);
    }
    
    Color m1 = { 140, 186, 112, 255 };
    Color m2 = { 110, 150, 80,  255 };
    for (float x = 0; x < worldWidth; x += 400) {
        DrawMountain({ x + 200, groundY }, 300, 180, m1);
        DrawMountain({ x + 50,  groundY }, 180, 120, m2);
    }
    
    for (const auto& c : clouds) c.Draw();
}
