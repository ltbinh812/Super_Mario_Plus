#include "Spring.h"
#include "Entity.h"
#include <string>

static const float BLOCK_SIZE = 32.0f;

Spring::Spring(Vector2 worldPos, SpringDir dir, float scale)
    : BaseItem(worldPos, BLOCK_SIZE, BLOCK_SIZE), dir_(dir) {}

Rectangle Spring::getSolidRect() const {
    return BaseItem::getHitbox(); // The actual solid bounds
}

Rectangle Spring::getHitbox() const {
    Rectangle rect = BaseItem::getHitbox();
    // Expand by 2 pixels to ensure overlap triggers onInteract even when player is pushed out of SolidRect
    rect.x -= 2.0f;
    rect.y -= 2.0f;
    rect.width += 4.0f;
    rect.height += 4.0f;
    return rect;
}

std::string Spring::idleFrame() const {
    switch (dir_) {
        case SpringDir::Down:  return "spring_down.png";
        case SpringDir::Up:    return "spring_up.png";
        case SpringDir::Left:  return "spring_left.png";
        case SpringDir::Right: return "spring_right.png";
    }
    return "spring_down.png";
}

std::string Spring::outFrame() const {
    switch (dir_) {
        case SpringDir::Down:  return "spring_out_down.png";
        case SpringDir::Up:    return "spring_out_up.png";
        case SpringDir::Left:  return "spring_out_left.png";
        case SpringDir::Right: return "spring_out_right.png";
    }
    return "spring_out_down.png";
}

void Spring::update(float dt) {
    BaseItem::update(dt);
    if (triggered_) {
        animTimer_ += dt;
        if (animTimer_ >= ANIM_DURATION) {
            triggered_ = false;
            animTimer_ = 0.0f;
        }
    }
}

void Spring::render(float alpha) {
    drawFrame(triggered_ ? outFrame() : idleFrame());
}

void Spring::onInteract(Entity& other) {
    if (triggered_) return;
    triggered_ = true;
    animTimer_ = 0.0f;

    auto& runtimeStats = other.getRuntimeStatsMutable();
    auto& vel = runtimeStats.velocity;
    
    // Khoá input ngang của người chơi trong 0.3s để lực đẩy không bị phím di chuyển đè lên
    runtimeStats.disableInputTimer = 0.3f;

    switch (dir_) {
        case SpringDir::Down:  vel.y = -LAUNCH_FORCE; break; // Base on ground, pushes UP
        case SpringDir::Up:    vel.y =  LAUNCH_FORCE; break; // Base on ceiling, pushes DOWN
        case SpringDir::Left:  vel.x =  LAUNCH_FORCE; break; // Base on left wall, pushes RIGHT
        case SpringDir::Right: vel.x = -LAUNCH_FORCE; break; // Base on right wall, pushes LEFT
    }
}
