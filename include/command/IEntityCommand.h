#pragma once

class Entity;

class IEntityCommand {
public:
    virtual ~IEntityCommand() = default;
    virtual void Execute(Entity& entity) = 0;
};
