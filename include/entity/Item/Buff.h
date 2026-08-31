#pragma once
#include "BaseItem.h"
#include <string>
#include <memory>

#include "IBuffEffect.h"

class Buff : public BaseItem {
public:
    Buff(Vector2 worldPos, float scale = 2.0f, const std::string& specificType = "");
    void render(float alpha) override;
    void onInteract(Entity& other) override;
    void forceInteract(Entity& other) override;
    const IBuffEffect& getEffect() const { return *effect_; }

private:
    std::unique_ptr<IBuffEffect> effect_;
};
