#pragma once
#include "IEntityState.h"

class Player;

class PlayerIdleState : public IEntityState<Player> {
    public:
    void onJump(Player& player) override;
    void onMoveRight(Player& player) override;
    void onMoveLeft(Player& player) override;
    void update(Player& player, float dt) override;
    void onEnter(Player& player) override;

};

class PlayerRunState : public IEntityState<Player> {
    public:
    void onJump(Player& player) override;
    void onMoveRight(Player& player) override;
    void onMoveLeft(Player& player) override;
    void onStopMove(Player& player) override;
    void update(Player& player, float dt) override;
    void onEnter(Player& player) override;

};

class PlayerJumpState : public IEntityState<Player> {
    public:
    void onMoveRight(Player& player) override;
    void onMoveLeft(Player& player) override;
    void update(Player& player, float dt) override;
    void onEnter(Player& player) override;

};

