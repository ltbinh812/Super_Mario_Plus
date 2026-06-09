#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "board.h"
#include "game.h"

using namespace std;

const int PLAYING_BUTTON_SIZE_WIDTH = 100;
const int PLAYING_BUTTON_SIZE_HIGHT = 20;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    SDL_Window* window = SDL_CreateWindow(
        "Go Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_SIZE, WINDOW_SIZE,
        SDL_WINDOW_SHOWN
    );
    

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);
    
    RUN_PLAYING(window, renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    // Quit subsystems
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}