#pragma once
#include "Entity.h"
#include "Item/AtlasAnimation.h"
#include <unordered_map>
#include <string>
#include <memory>

class IMobState;

class Mob : public Entity {
protected:
    std::unique_ptr<IMobState> currentState;
    std::unordered_map<std::string, AtlasAnimation> animations;
    AtlasAnimation* currentAnim;
    std::string mobType;
    
    // AI Variables
    float stateTimer;
    Vector2 spawnPoint;
    bool isFacingRight;
    bool isDead;
    float hurtTimer;

public:
    Mob(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats);
    ~Mob() override;

    void update(float dt) override;
    void render(float alpha) override;

    // AI Processing
    void decideAction();
    void process();
    
    // Combat
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;
    bool getIsActive() const override;
    
    // Map collisions
    void onHitWall(bool rightWall) override;
    void onLand(float floorY) override;

    // Mob-specific methods
    void changeState(std::unique_ptr<IMobState> newState);
    void setAnimation(const std::string& animName);
    
    // Utility
    const std::string& getMobType() const { return mobType; }
    bool getIsFacingRight() const { return isFacingRight; }
    void setFacingRight(bool right) { isFacingRight = right; }
    float getStateTimer() const { return stateTimer; }
    void addStateTimer(float dt) { stateTimer += dt; }
    void resetStateTimer() { stateTimer = 0.0f; }
    
    Vector2 getPosition() const { return worldStats.position; }
    void setVelocity(Vector2 v) { runtimeStats.velocity = v; }
    Vector2 getVelocity() const { return runtimeStats.velocity; }
    
    bool isHurt() const { return hurtTimer > 0.0f; }
    bool checkIsDead() const { return isDead; }
    
    AtlasAnimation* getCurrentAnim() { return currentAnim; }
};
