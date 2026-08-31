#pragma once
#include "IItemUseStrategy.h"
#include "Player.h"
#include "SpawnCommand.h"
#include <iostream>

class ThrowBombStrategy : public IItemUseStrategy {
public:
    void use(Player& player) override {
        auto queue = player.getCommandQueue();
        if (!queue) return;
        
        const float THROW_VX = 350.0f;
        const float THROW_VY = -280.0f;
        
        SpawnCommand cmd;
        cmd.category       = SpawnCategory::Item;
        cmd.itemIdentifier = "ThrownBoom";
        cmd.position       = {player.getWorldStats().position.x, player.getWorldStats().position.y - 16.0f};
        cmd.velocity       = {player.getWorldStats().isFacingRight ? THROW_VX : -THROW_VX, THROW_VY};
        queue->push(cmd);
        
        std::cout << "[ThrowBombStrategy] Threw Boom!\n";
    }
};
