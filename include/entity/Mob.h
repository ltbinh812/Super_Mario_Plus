#pragma once
#include "Entity.h"
#include "Item/AtlasAnimation.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "Animation.h"
#include "Skill/IEnemySkill.h"

class IMobState;

struct MobAttackData {
    int damage;
    int hitboxStartFrame;
    int hitboxEndFrame;
    int hitboxTotalFrames;
    float frameTime;
    Rectangle box;
};

struct MobAIData {
    float detectionRange;
    float attackRange;
    float patrolSpeed;
    float patrolTime;
};

struct MobConfig {
    std::string name;
    MobAttackData attackData;
    MobAIData aiData;
};

class Mob : public Entity {
protected:
    std::unique_ptr<IMobState> currentState;
    std::unordered_map<std::string, AtlasAnimation> animations;
    AtlasAnimation* currentAnim;
    
    // New Standard Animation and Skill system
    std::unordered_map<std::string, Animation> standardAnimations;
    Animation* currentStandardAnim = nullptr;
    std::vector<std::unique_ptr<IEnemySkill>> enemySkills;
    
    std::string mobType;
    MobConfig config;
    
    // AI Variables
    float stateTimer;
    float aggroCooldown = 0.0f;
    float attackCooldown = 0.0f;
    Vector2 spawnPoint;
    bool isFacingRight;
    bool isDead;
    float hurtTimer;
    float deadTimer;
    
    std::vector<class Player*> targetPlayers;
    bool isHitboxActive;
    Hitbox currentHitbox;

public:
    Mob(Vector2 worldPos, const std::string& mobType, const CharacterBaseStats& bStats, const MobConfig& config);
    ~Mob() override;

    void update(float dt) override;
    void render(float alpha) override;

    // AI Processing
    void decideAction();
    void process();
    
    // Combat
    void takeDamage(int damage, float knockbackDirX = 0.0f, bool forceInterrupt = true) override;
    bool getIsActive() const override;
    
    void setTargetPlayers(const std::vector<class Player*>& players) override { targetPlayers = players; }
    class Player* getClosestPlayer() const;
    
    bool hasActiveHitbox() const override { return isHitboxActive; }
    Hitbox getActiveHitbox() override { return currentHitbox; }
    void setHitboxActive(bool active, Hitbox hb = {}) { isHitboxActive = active; currentHitbox = hb; }
    
    // Map collisions
    void onHitWall(bool rightWall, bool isCliff = false) override;
    void onLand(float floorY) override;

    // Mob-specific methods
    void changeState(std::unique_ptr<IMobState> newState);
    void setAnimation(const std::string& animName);
    
    // Standard asset methods
    void setStandardAnimations(std::unordered_map<std::string, Animation> anims) {
        standardAnimations = std::move(anims);
    }
    void addEnemySkill(std::unique_ptr<IEnemySkill> skill) {
        enemySkills.push_back(std::move(skill));
    }
    const std::vector<std::unique_ptr<IEnemySkill>>& getEnemySkills() const {
        return enemySkills;
    }
    bool hasStandardAnimations() const {
        return !standardAnimations.empty();
    }
    
    // Utility
    const std::string& getMobType() const { return mobType; }
    const MobConfig& getConfig() const { return config; }
    bool getIsFacingRight() const { return isFacingRight; }
    void setFacingRight(bool right) { isFacingRight = right; }
    float getStateTimer() const { return stateTimer; }
    void addStateTimer(float dt) { stateTimer += dt; }
    void resetStateTimer() { stateTimer = 0.0f; }
    
    float getAggroCooldown() const { return aggroCooldown; }
    void setAggroCooldown(float t) { aggroCooldown = t; }
    
    float getAttackCooldown() const { return attackCooldown; }
    void setAttackCooldown(float t) { attackCooldown = t; }
    
    Vector2 getPosition() const { return worldStats.position; }
    void setVelocity(Vector2 v) { runtimeStats.velocity = v; }
    Vector2 getVelocity() const { return runtimeStats.velocity; }
    
    bool isHurt() const { return hurtTimer > 0.0f; }
    bool checkIsDead() const { return isDead; }
    
    AtlasAnimation* getCurrentAnim() { return currentAnim; }
    Animation* getCurrentStandardAnim() { return currentStandardAnim; }
    
    // Sound Management
    void playSound(const std::string& soundKey, bool loop = true);
    void updateSound();
    
    void setSoundFrames(std::unordered_map<std::string, std::unordered_map<int, std::string>> frames) { soundFrames = std::move(frames); }
    const std::unordered_map<std::string, std::unordered_map<int, std::string>>& getSoundFrames() const { return soundFrames; }

protected:
    std::string currentBaseAnimName = "";
    bool currentAnimLooping = false;
    float maxHearingDistance = 800.0f; // Distance at which sound fades to 0
    float idleSoundTimer = 0.0f;
    
    std::unordered_map<std::string, std::unordered_map<int, std::string>> soundFrames;
    int lastSoundFrameIndex = -1;
};
