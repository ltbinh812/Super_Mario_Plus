#pragma once

class Player;

class IPlayerCommand {
public:
    virtual ~IPlayerCommand() = default;
    virtual void Execute(Player& player) = 0;
};
