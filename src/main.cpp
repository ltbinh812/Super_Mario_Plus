#include "SuperMarioPlus.h"

int main() {
    SuperMarioPlus game;
    game.Init();

    while (!game.ShouldClose()) {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}
