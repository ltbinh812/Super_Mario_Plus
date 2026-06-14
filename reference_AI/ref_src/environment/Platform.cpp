#include "Platform.h"

Platform::Platform(Rectangle r, Color c) : rect_(r), color_(c) {}

void Platform::Draw() const {
    DrawRectangleRec(rect_, color_);
    Color dark = { (unsigned char)(color_.r * 0.7f),
                   (unsigned char)(color_.g * 0.7f),
                   (unsigned char)(color_.b * 0.7f), 255 };
    DrawRectangle((int)rect_.x, (int)rect_.y + 16,
                  (int)rect_.width, (int)rect_.height - 16, dark);
    Color top = { (unsigned char)(color_.r * 1.1f > 255 ? 255 : color_.r * 1.1f),
                  (unsigned char)(color_.g * 1.1f > 255 ? 255 : color_.g * 1.1f),
                  (unsigned char)(color_.b * 1.1f > 255 ? 255 : color_.b * 1.1f), 255 };
    DrawRectangle((int)rect_.x, (int)rect_.y, (int)rect_.width, 16, top);
}
