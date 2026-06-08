#include "SuperMarioPlus.h"

int main() {
  SuperMarioPlus game;
  game.Init();

  while (!game.ShouldClose()) {
    game.Update();
    game.Draw();
  }
  std::cout << "test\n";
  CloseWindow();
  return 0;
}
