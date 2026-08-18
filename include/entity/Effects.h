#pragma once
#include <string>

class Entity;

class IEffect {
public:
    virtual ~IEffect() = default;
    
    // Updates the effect. Returns true if the effect is finished and should be removed.
    virtual bool update(Entity& entity, float dt) = 0;
    
    // Called when the same effect is applied again (to reset duration or stack)
    virtual void refresh() = 0;
    
    virtual std::string getName() const = 0;
    virtual void render(const Entity& entity, float alpha) {}
};

class PoisonEffect : public IEffect {
private:
    float duration = 5.0f;
    float tickTimer = 0.0f;
    bool inPoison = true;
    float animTimer1 = 0.0f;
    float animTimer2 = 0.0f;
    int currentFrame1 = 0;
    int currentFrame2 = 0;
public:
    bool update(Entity& entity, float dt) override;
    void refresh() override;
    void setInPoison(bool value) { inPoison = value; }
    std::string getName() const override { return "Poison"; }
    void render(const Entity& entity, float alpha) override;
};

class LavaEffect : public IEffect {
private:
    float duration = 5.0f;
    float tickTimer = 0.0f;
    float animTimer = 0.0f;
    int currentFrame = 0;
    bool inLava = true;
public:
    bool update(Entity& entity, float dt) override;
    void refresh() override;
    void setInLava(bool value) { inLava = value; }
    std::string getName() const override { return "Lava"; }
    void render(const Entity& entity, float alpha) override;
};
