#include "Decoration.h"

Decoration::Decoration(Vector2 pos, int type) : pos_(pos), type_(type) {}

void Decoration::Draw() const {
    switch (type_) {
        case 0: DrawBush(1.0f); break;
        case 1: DrawTree(1.0f); break;
        case 2: DrawPipe(64.0f); break;
    }
}

void Decoration::DrawBush(float scale) const {
    Color col  = {  60, 160,  50, 255 };
    Color col2 = {  40, 120,  30, 255 };
    int x = (int)pos_.x, y = (int)pos_.y;
    DrawCircle(x,      y - (int)(18 * scale), (int)(20 * scale), col2);
    DrawCircle(x + (int)(20 * scale), y - (int)(14 * scale), (int)(18 * scale), col);
    DrawCircle(x - (int)(20 * scale), y - (int)(14 * scale), (int)(16 * scale), col);
    DrawCircle(x,      y - (int)(22 * scale), (int)(16 * scale), col);
}

void Decoration::DrawTree(float scale) const {
    Color trunk  = { 120,  70, 20, 255 };
    Color leaves = {  50, 160, 40, 255 };
    Color leafD  = {  30, 120, 25, 255 };
    int x = (int)pos_.x, y = (int)pos_.y;
    int tw = (int)(16 * scale), th = (int)(50 * scale);
    DrawRectangle(x - tw / 2, y - th, tw, th, trunk);
    DrawTriangle({ (float)x, (float)(y - th - (int)(60 * scale)) },
                 { (float)(x - (int)(40 * scale)), (float)(y - th) },
                 { (float)(x + (int)(40 * scale)), (float)(y - th) }, leaves);
    DrawTriangle({ (float)x, (float)(y - th - (int)(90 * scale)) },
                 { (float)(x - (int)(32 * scale)), (float)(y - th - (int)(40 * scale)) },
                 { (float)(x + (int)(32 * scale)), (float)(y - th - (int)(40 * scale)) }, leafD);
}

void Decoration::DrawPipe(float height) const {
    Color pipeG  = {  60, 170,  50, 255 };
    Color pipeD  = {  30, 120,  25, 255 };
    int x = (int)pos_.x, y = (int)pos_.y;
    int pw = 48, bw = 56, bh = 16;
    DrawRectangle(x - pw / 2, y - (int)height, pw, (int)height, pipeD);
    DrawRectangle(x - pw / 2, y - (int)height, pw / 3, (int)height, pipeG);
    DrawRectangle(x - bw / 2, y - (int)height - bh, bw, bh, pipeG);
    DrawRectangle(x - bw / 2, y - (int)height - bh, bw / 3, bh, { 100, 220, 80, 255 });
}
