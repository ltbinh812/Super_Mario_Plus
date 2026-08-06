#pragma once
#include "SpawnCommand.h"
#include <vector>

class CommandQueue {
private:
    std::vector<SpawnCommand> queue;

public:
    void push(const SpawnCommand& cmd) {
        queue.push_back(cmd);
    }

    std::vector<SpawnCommand> popAll() {
        std::vector<SpawnCommand> result = std::move(queue);
        queue.clear();
        return result;
    }
};
