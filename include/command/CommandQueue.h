#pragma once
#include "SpawnCommand.h"
#include <vector>
#include <algorithm>

class CommandQueue {
private:
    std::vector<SpawnCommand> queue;

public:
    void push(const SpawnCommand& cmd) {
        queue.push_back(cmd);
    }

    // Drain ALL commands (for Update: spawn, etc.)
    std::vector<SpawnCommand> popAll() {
        std::vector<SpawnCommand> result = std::move(queue);
        queue.clear();
        return result;
    }

    // Drain only commands of a specific category (used by Process for ExplosionDamage)
    std::vector<SpawnCommand> peekAndConsumeByCategory(SpawnCategory cat) {
        std::vector<SpawnCommand> result;
        std::vector<SpawnCommand> remaining;
        for (auto& cmd : queue) {
            if (cmd.category == cat) result.push_back(std::move(cmd));
            else                      remaining.push_back(std::move(cmd));
        }
        queue = std::move(remaining);
        return result;
    }
};
