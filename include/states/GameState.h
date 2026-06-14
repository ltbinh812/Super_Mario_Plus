#ifndef GAMESTATE_H
#define GAMESTATE_H

class GameState {
public:
    virtual ~GameState() = default;
    virtual void HandleInput() = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
};

#endif
