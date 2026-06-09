#include <vector>
#include <iostream>
#include <stack>
#include <thread>
#include <atomic>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "board.h"
#include "move.h"
#include "history.h"
#include "button.h"
#include "game.h"
#include "playing_interface.h"
#include "skip.h"
#include "check_game_state.h"
#include "save_load_game.h"
#include "loadgame_interface.h"
#include "endgame_interface.h"
#include "home_interface.h"
#include "playing_mode_interface.h"
#include "menu_interface.h"
#include "text_utlish.h"
#include "easy_mode.h"
#include "medium_mode.h"
#include "hard_mode.h"
#include "choose_mode_interface.h"
using namespace std;

int BOARD_SIZE = 13;
int CELL_SIZE = 63;
int STONE_RADIUS = 20;
int CLICK_RADIUS = 22; 
 
int MARGIN = 123;
const int BOARD_LENGTH = 72 * 7; // 72 = lcm(8, 18)

// AI Thread management
static atomic<bool> g_ai_thinking(false);
static thread g_ai_thread;
const int WINDOW_SIZE = 700;

void init_game(bool &blackTurn, int &who_plays_first, KataGoAI &katago, bool AI_enabled = true) {
    blackTurn = true;
    who_plays_first = rand() % 2 + 1;
    init_board(BOARD_SIZE, CELL_SIZE, STONE_RADIUS, CLICK_RADIUS, MARGIN, BOARD_LENGTH);
    Init_Player();
    init_skip();
    Init_History(blackTurn, who_plays_first);
    if (AI_enabled) 
        katago.init(BOARD_SIZE);
}

int RUN_PLAYING(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_SetWindowSize(window, WINDOW_SIZE, WINDOW_SIZE);
    SDL_Texture* black_stone = IMG_LoadTexture(renderer, "../assets/black.png");
    SDL_Texture* white_stone = IMG_LoadTexture(renderer, "../assets/white.png");
    TTF_Font* font1 = TTF_OpenFont("../assets/BAUHS93.ttf", 24);
    TTF_Font* font2 = TTF_OpenFont("../assets/BAUHS93.ttf", 48);
    TTF_Font* font3 = TTF_OpenFont("../assets/BAUHS93.ttf", 36);

    SDL_Color color = {0, 0, 0};

    if (!black_stone) {
        SDL_Log("Failed to load black stone images");
        return -1;
    }
    if (!white_stone) {
        SDL_Log("Failed to load white stone images");
        return -1;
    }
    
    if (!font2) {
        SDL_Log("Failed to load font");
        return -1;
    }

    Button undo_button(10, WINDOW_SIZE - 60, 60, 40, "../assets/undo.png", renderer, "Undo");
    Button redo_button(80, WINDOW_SIZE - 60, 60, 40, "../assets/redo.png", renderer, "Redo");
    Button pass_button(WINDOW_SIZE / 2 - 40, WINDOW_SIZE - 65, 120, 42, "../assets/pass.png", renderer, "Pass");
    Button menu_button(5, 5, 120, 40, "../assets/menu.png", renderer, "Menu");
    Button start_button(WINDOW_SIZE / 2 - 250 / 2, 187, 250, 75, "../assets/start.png", renderer, "Start Game");
    Button loadgame_button(WINDOW_SIZE / 2 - 250 / 2, 287, 250, 75, "../assets/loadgame.png", renderer, "Load Game");
    Button options_button(WINDOW_SIZE / 2 - 250 / 2, 387, 250, 75, "../assets/options.png", renderer, "Options");
    Button exit_button(WINDOW_SIZE / 2 - 250 / 2, 487, 250, 75, "../assets/exit.png", renderer, "Exit");
    Button back_button(10, 10, 60, 60, "../assets/back.png", renderer, "Back");
    Button home_button1(5, 5, 150, 80, "../assets/home.png", renderer, "Home1");
    Button home_button2(WINDOW_SIZE / 2 - 250 / 2 - 8, 377 - 130, 273, 130, "../assets/home.png", renderer, "Home2");
    Button one_player_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 - 80, 195, 85, "../assets/2players_p.png", renderer, "1 player mode");
    Button two_players_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 + 40, 195, 85, "../assets/1player_p.png", renderer, "2 players mode");
    Button board_mode_19_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 - 80, 200, 75, "../assets/board_19x19.png", renderer, "Board 19x19");
    Button board_mode_13_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 + 40, 200, 75, "../assets/board_13x13.png", renderer, "Board 13x13");
    Button sound_effect_button(135, 100, 120, 60, "../assets/sound_effect.png", renderer, "Sound Effect");
    Button music_button(135, 190, 120, 60, "../assets/music.png", renderer, "Music");
    Button increase_music_button(485, 200, 30, 20, "../assets/increase.png", renderer, "Increase Music"); // 435 200
    Button increase_sound_effect_button(485, 110, 30, 20, "../assets/increase.png", renderer, "Increase Sound Effect"); // 435 110
    Button decrease_music_button(435, 200, 30, 20, "../assets/decrease.png", renderer, "Decrease Music"); // 485 200
    Button decrease_sound_effect_button(435, 110, 30, 20, "../assets/decrease.png", renderer, "Decrease Sound Effect"); // 485 110
    // button for changing playing background color theme, position is under music and sound effect buttons, respectively, on one line
    
    Button button_color_rice_paper(135, 350, 150, 40, "../assets/rice_paper.png", renderer, "Rice Paper");
    Button button_color_soft_mist(300, 350, 150, 40, "../assets/soft_mist.png", renderer, "Soft Mist");
    Button button_color_wood(465, 350, 150, 40, "../assets/wood.png", renderer, "Wood");
    Button easy_mode_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 - 113, 250, 75,"../assets/easy.png", renderer, "Easy Mode"); 
    Button medium_mode_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2, 250, 75, "../assets/medium.png", renderer, "Medium Mode");   
    Button hard_mode_button(WINDOW_SIZE / 2 - 250 / 2, WINDOW_SIZE / 2 + 113, 250, 75, "../assets/hard.png", renderer, "Hard Mode");
    
    // place right down corner
    Button save_button(WINDOW_SIZE - 100, WINDOW_SIZE - 60, 95, 45, "../assets/save.png", renderer, "Save");
    Button reset_button(WINDOW_SIZE - 200, WINDOW_SIZE - 60, 95, 45, "../assets/reset.png", renderer, "Reset");
    Button yes_button(WINDOW_SIZE / 2 - 150 - 20, WINDOW_SIZE / 2 + 50, 150, 60, "../assets/yes.png", renderer, "Yes");
    Button no_button(WINDOW_SIZE / 2 + 20, WINDOW_SIZE / 2 + 50, 150, 60, "../assets/no.png", renderer, "No");

    SDL_Texture* bg_rice_paper = IMG_LoadTexture(renderer, "../assets/rice_paper.png");
    SDL_Texture* bg_soft_mist = IMG_LoadTexture(renderer, "../assets/soft_mist.png");
    SDL_Texture* bg_wood = IMG_LoadTexture(renderer, "../assets/wood.png");
    SDL_Texture* current_background = bg_wood;

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    LongSound bgm(Mix_LoadMUS("../assets/bgmusic.wav"), 5, false);
    ShortSound place_stone_sound(Mix_LoadWAV("../assets/stone_effect.wav"), 5, false);
    bgm.play();

    srand(time(0));
    bool running = true;
    bool blackTurn = true;
    int who_plays_first; // 1: ai, 2: player
    int hoverRow = -1, hoverCol = -1;
    Uint32 endgame_time;

    GameState current_state = GameState::HOME;
    GameState next_state = current_state;
    BackgroundTheme current_theme = BackgroundTheme::WOOD;
    AIState ai_state = AIState::NONE;

    stack<GameState> stackState;

    KataGoAI katago;

    vector<Button*> playing_button_list = {
        &undo_button,
        &redo_button, 
        &pass_button,
        &menu_button,
        &save_button,
        &reset_button
    };
    
    vector<Button*> endgame_button_list = {
        &home_button1
    };

    vector<Button*> loadgame_button_list(SIZE_LIST_SHOW);
    for(int i = 0; i < SIZE_LIST_SHOW; i++){
        loadgame_button_list[i] = new Button(WINDOW_SIZE / 2 - BUTTON_WIDTH / 2,
                                             (WINDOW_SIZE - LIST_LOAD_LENGTH) / 2 + i * BUTTON_HEIGHT * 3 / 2,
                                             BUTTON_WIDTH, 
                                             BUTTON_HEIGHT,
                                             "../assets/bar.png",
                                             renderer,
                                             "");
    }

    vector<Button*> home_button_list{
        &start_button,
        &loadgame_button,
        &options_button,
        &exit_button
    };

    vector<Button*> playing_mode_button_list{
        &back_button,
        &one_player_button,
        &two_players_button
    };

    vector<Button*> menu_button_list{
        &back_button,
        &home_button2,
        &options_button,
    };

    vector<Button*> board_mode_button_list{
        &back_button,
        &board_mode_19_button,
        &board_mode_13_button
    };

    vector<Button*> options_button_list{
        &back_button,
        &music_button,
        &sound_effect_button,
        &increase_music_button,
        &decrease_music_button,
        &increase_sound_effect_button,
        &decrease_sound_effect_button,
        &button_color_rice_paper,
        &button_color_soft_mist,
        &button_color_wood
    };

    vector<Button*> choose_mode_button_list{
        &back_button,
        &easy_mode_button,
        &medium_mode_button,
        &hard_mode_button
    };

    vector<Button*> confirm_button_list{
        &back_button,
        &yes_button,
        &no_button
    };

    SDL_Event e;
    SDL_Surface* loadedSurface = IMG_Load("../assets/background.png");
    if (loadedSurface == nullptr) {
        std::cerr << "Lỗi khi load ảnh: " << IMG_GetError() << std::endl;
        // Có thể return hoặc xử lý lỗi khác ở đây
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT){
                running = false;
                return 0;
            }
            
            if (current_state == GameState::HOME) {
                for (auto btn : home_button_list) {
                    btn->handleEvent(e);
                }

                if (start_button.clicked(e)) {
                    stackState.push(current_state);
                    current_state = GameState::BOARD_MODE;
                    break;
                }

                if (loadgame_button.clicked(e)) {
                    stackState.push(current_state);
                    current_state = GameState::LOAD_GAME;
                    init_loadgame_interface();
                    break;
                }

                if (options_button.clicked(e)) {
                    // Handle options button click
                    stackState.push(current_state);
                    current_state = GameState::OPTION;
                    break;
                }

                if (exit_button.clicked(e)) {
                    running = false;
                    current_state = GameState::NONE;
                    break;
                }
            }
            
            if (current_state == GameState::BOARD_MODE){
                for (auto btn : board_mode_button_list)
                    btn->handleEvent(e);
                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                    break;
                }

                if (board_mode_19_button.clicked(e)){
                    BOARD_SIZE = 19;
                    stackState.push(current_state);
                    current_state = GameState::GAME_MODE;
                    break;
                }

                if (board_mode_13_button.clicked(e)){
                    BOARD_SIZE = 13;
                    stackState.push(current_state);
                    current_state = GameState::GAME_MODE;
                    break;
                }
            }

            if (current_state == GameState::GAME_MODE){
                for (auto btn : playing_mode_button_list)
                    btn->handleEvent(e);

                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                    break;
                }

                if (one_player_button.clicked(e)){
                    current_state = GameState::CHOOSE_MODE;
                    break;
                }

                if (two_players_button.clicked(e)){
                    init_game(blackTurn, who_plays_first, katago, false);
                    current_state = GameState::PLAYING;
                    ai_state = AIState::NONE;
                    while (stackState.size() > 0)
                        stackState.pop();
                    break;
                }
            }

            if(current_state == GameState::CHOOSE_MODE){
                for(auto btn: choose_mode_button_list)
                    btn->handleEvent(e);
                
                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                    break;
                }
                if (easy_mode_button.clicked(e)){
                    current_state = GameState::PLAYING;
                    ai_state = AIState::EASY_PLAY;
                    while (stackState.size() > 0)
                        stackState.pop();
                    init_game(blackTurn, who_plays_first, katago);
                    break;
                }
                if (medium_mode_button.clicked(e)){
                    current_state = GameState::PLAYING;
                    ai_state = AIState::MEDIUM_PLAY;
                    while (stackState.size() > 0)
                        stackState.pop();
                    init_game(blackTurn, who_plays_first, katago);
                    break;
                }
                if (hard_mode_button.clicked(e)){
                    current_state = GameState::PLAYING;
                    ai_state = AIState::HARD_PLAY;
                    while (stackState.size() > 0)
                        stackState.pop();
                    init_game(blackTurn, who_plays_first, katago);
                    break;
                }
            }

            if (current_state == GameState::PLAYING) {
                // cout << blackTurn << " " << who_plays_first << endl;
                board_handle_event(e, hoverRow, hoverCol);
                for (auto btn : playing_button_list) {
                    btn->handleEvent(e);
                }

                // Player's turn 
                if(ai_state == AIState::NONE){
                    if (undo_button.clicked(e)) {
                        Undo_Move(blackTurn, who_plays_first);
                    }
                    if (redo_button.clicked(e)) {
                        Redo_Move(blackTurn, who_plays_first);
                    }
                    if (pass_button.clicked(e)) {
                        skip_turn(blackTurn, who_plays_first);
                    }
                    if (menu_button.clicked(e)) {
                        stackState.push(current_state);
                        current_state = GameState::MENU;
                        break;
                    }
                    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                        if (hoverRow != -1 && board[hoverRow][hoverCol] == EMPTY){
                            make_move(e, board, blackTurn, who_plays_first, katago, ai_state, place_stone_sound);
                        }
                    }
                }
                else{
                    // Player's turn (only when AI is not thinking)
                    if(who_plays_first == 2 && !g_ai_thinking){
                        if (undo_button.clicked(e)) {
                            if (ai_state == AIState::HARD_PLAY) {
                                katago.undoMove();  // Undo AI move
                                katago.undoMove();  // Undo player move
                            }
                            Undo_Move(blackTurn, who_plays_first);
                            Undo_Move(blackTurn, who_plays_first);
                        }
                        if (redo_button.clicked(e)) {
                            if (ai_state == AIState::HARD_PLAY) {
                                katago.redoMoves(2);  // Redo 2 moves (player + AI)
                            }
                            Redo_Move(blackTurn, who_plays_first);
                            Redo_Move(blackTurn, who_plays_first);
                        }
                        if (pass_button.clicked(e)) {
                            skip_turn(blackTurn, who_plays_first);
                        }
                        if (menu_button.clicked(e)) {
                            stackState.push(current_state);
                            current_state = GameState::MENU;
                            break;
                        }

                        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                            if (hoverRow != -1 && board[hoverRow][hoverCol] == EMPTY){
                                make_move(e, board, blackTurn, who_plays_first, katago, ai_state, place_stone_sound);
                            }
                        }
                    }
                    // AI turn 
                    else if (who_plays_first == 1) {                        
                        if(ai_state == AIState::EASY_PLAY){
                            easy_mode_move(blackTurn, who_plays_first);
                        }
                        else if(ai_state == AIState::MEDIUM_PLAY){
                            medium_mode_move(blackTurn, who_plays_first);
                        }
                        else if(ai_state == AIState::HARD_PLAY && !g_ai_thinking){
                            g_ai_thinking = true;
                            if (g_ai_thread.joinable()) {
                                g_ai_thread.join();
                            }
                            g_ai_thread = thread([&blackTurn, &who_plays_first, &katago, ai_state]() {
                                hard_mode_move(blackTurn, who_plays_first, katago);
                                g_ai_thinking = false;
                            });
                        }
                    }
                }

                // Block save/reset when AI is thinking
                if (!g_ai_thinking) {
                    if (save_button.clicked(e)) {
                        SaveGame(blackTurn, who_plays_first, ai_state);
                        break;
                    }

                    if (reset_button.clicked(e)){
                        stackState.push(current_state);
                        current_state = GameState::CONFIRM;
                        break;
                    }
                }
            }

            if (current_state == GameState::CONFIRM){
                for (auto btn : confirm_button_list)
                    btn->handleEvent(e);
                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                    break;
                }
                if (yes_button.clicked(e)){
                    init_game(blackTurn, who_plays_first, katago, ai_state != AIState::NONE);
                    current_state = GameState::PLAYING;
                    stackState.pop();
                    break;
                }
                if (no_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                    break;
                }
            }

            if (current_state == GameState::MENU){
                for (auto btn : menu_button_list)
                    btn->handleEvent(e);
                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                }
                if (home_button2.clicked(e)){
                    current_state = GameState::HOME;
                    while (stackState.size() > 0){
                        stackState.pop();
                    }
                }
                if (options_button.clicked(e)){
                    stackState.push(current_state);
                    current_state = GameState::OPTION;
                }
            }

            if (current_state == GameState::OPTION){
                for (auto btn : options_button_list)
                    btn->handleEvent(e);
                if (back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                }

                if (music_button.clicked(e)){
                    bgm.switchMute();
                }

                if (sound_effect_button.clicked(e)){
                    // Toggle sound effect on/off
                    place_stone_sound.switchMute();
                }

                if (increase_music_button.clicked(e)){
                    bgm.adjustVolume(1);
                }   
                if (decrease_music_button.clicked(e)){
                    bgm.adjustVolume(-1);
                }
                if (increase_sound_effect_button.clicked(e)){
                    place_stone_sound.adjustVolume(1);
                }
                if (decrease_sound_effect_button.clicked(e)){
                    place_stone_sound.adjustVolume(-1);
                }

                if (button_color_rice_paper.clicked(e)){
                    current_theme = BackgroundTheme::RICE_PAPER;
                    current_background = bg_rice_paper;
                }
                if (button_color_soft_mist.clicked(e)){
                    current_theme = BackgroundTheme::SOFT_MIST;
                    current_background = bg_soft_mist;
                }
                if (button_color_wood.clicked(e)){
                    current_theme = BackgroundTheme::WOOD;
                    current_background = bg_wood;
                }
            }

            if(current_state == GameState::LOAD_GAME){
                vector<string> save_files = allFileLoadGame();
                load_handle_event(renderer, e, save_files, loadgame_button_list);
                for (int i = 0; i < SIZE_LIST_SHOW; i++){
                    if (start_index + i >= save_files.size()) break;
                    loadgame_button_list[i]->handleEvent(e);
                }

                back_button.handleEvent(e);
                for(int i = 0; i < SIZE_LIST_SHOW; i++){
                    if (start_index + i >= save_files.size()) break;
                    if(loadgame_button_list[i]->clicked(e)){
                        LoadGame(blackTurn, who_plays_first, ai_state, save_files[start_index + i]);
                        if(ai_state == AIState::HARD_PLAY){
                            katago.init(BOARD_SIZE);
                            katago.syncBoard(board, blackTurn);
                        }
                        current_state = GameState::PLAYING;
                        break;
                    }
                }
                if(back_button.clicked(e)){
                    current_state = stackState.top();
                    stackState.pop();
                }                
            }

            if (current_state == GameState::END_GAME) {
                for (auto btn : endgame_button_list) {
                    btn->handleEvent(e);
                }
                if (home_button1.clicked(e)) {
                    current_state = GameState::HOME;
                    while (stackState.size() > 0)
                        stackState.pop();
                }
            }
        }
        
        
        // MENU
        // draw board background
        if (current_state == GameState::HOME) {
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_home_interface(renderer, home_button_list);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::GAME_MODE){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_playing_mode_interface(renderer, playing_mode_button_list);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::BOARD_MODE){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_board_mode_interface(renderer, board_mode_button_list);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if(current_state == GameState::CHOOSE_MODE){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_choose_mode_interface(renderer, choose_mode_button_list, back_button);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::PLAYING){
            // SDL_SetRenderDrawColor(renderer, 40, 44, 52, 255);
            // SDL_SetRenderDrawColor(renderer, 245, 240, 225, 255);
            // SDL_SetRenderDrawColor(renderer, 210, 220, 230, 255);
            // SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
            if (current_theme == BackgroundTheme::RICE_PAPER) {
                SDL_SetRenderDrawColor(renderer, 245, 240, 225, 255);
            } else if (current_theme == BackgroundTheme::SOFT_MIST) {
                SDL_SetRenderDrawColor(renderer, 210, 220, 230, 255);
            } else if (current_theme == BackgroundTheme::WOOD) {
                SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
            }
            SDL_RenderClear(renderer);
                
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
            SDL_Rect boardRect = {MARGIN - CELL_SIZE / 2, MARGIN - CELL_SIZE / 2, CELL_SIZE * BOARD_SIZE, CELL_SIZE * BOARD_SIZE};
            SDL_RenderFillRect(renderer, &boardRect);
            if(g_ai_thinking)
                draw_playing_interface(renderer, black_stone, white_stone, hoverRow, hoverCol, !blackTurn, playing_button_list, font1, color);
            else
                draw_playing_interface(renderer, black_stone, white_stone, hoverRow, hoverCol, blackTurn, playing_button_list, font1, color);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::MENU){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_menu_interface(renderer, menu_button_list);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    
        if (current_state == GameState::OPTION){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_options_interface(renderer, options_button_list, bgm.volume * (bgm.muted == 0), place_stone_sound.volume * (place_stone_sound.muted == 0), font1, color, current_background);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::CONFIRM){
            SDL_Rect destRect;
            destRect.x = 0;
            destRect.y = 0;
            destRect.w = WINDOW_SIZE;  // Chiều rộng đích = Chiều rộng cửa sổ
            destRect.h = WINDOW_SIZE; // Chiều cao đích = Chiều cao cửa sổ
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 200);
            SDL_RenderFillRect(renderer, &destRect);

            draw_confirm_interface(renderer, confirm_button_list, font3, color, "Are you sure you want to reset the game?");

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        if (current_state == GameState::LOAD_GAME){
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
            SDL_RenderClear(renderer);

            vector<string> save_files = allFileLoadGame();
            load_handle_event(renderer, e, save_files, loadgame_button_list);
            draw_loadgame_interface(renderer, save_files, loadgame_button_list, back_button, font1, color);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
            // draw_list_load_game(rendere);
        }

        if (current_state == GameState::END_GAME) {
            Uint32 current_time = SDL_GetTicks();
            if (current_time - endgame_time < 1500) {
                if (current_theme == BackgroundTheme::RICE_PAPER) {
                    SDL_SetRenderDrawColor(renderer, 245, 240, 225, 255);
                } else if (current_theme == BackgroundTheme::SOFT_MIST) {
                    SDL_SetRenderDrawColor(renderer, 210, 220, 230, 255);
                } else if (current_theme == BackgroundTheme::WOOD) {
                    SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
                }
                SDL_RenderClear(renderer);
                
                SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
                SDL_Rect boardRect = {MARGIN - CELL_SIZE / 2, MARGIN - CELL_SIZE / 2, CELL_SIZE * BOARD_SIZE, CELL_SIZE * BOARD_SIZE};
                SDL_RenderFillRect(renderer, &boardRect);
                draw_playing_interface(renderer, black_stone, white_stone, hoverRow, hoverCol, blackTurn, playing_button_list, font1, color);
                
                
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // 50% transparent black
                SDL_Rect bgRect = {0, 0, WINDOW_SIZE, WINDOW_SIZE};
                SDL_RenderFillRect(renderer, &bgRect);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                SDL_Texture* KO_threat = nullptr;
                SDL_Color textColor = {255, 0, 0, 255};
                string KO_threat_text = "GAME OVER";
                KO_threat = renderText(renderer, font2, KO_threat_text, textColor);
                
                int w, h;
                SDL_QueryTexture(KO_threat, NULL, NULL, &w, &h);
                SDL_Rect dst = {(WINDOW_SIZE - w) / 2, (WINDOW_SIZE - h) / 2, w, h};
                SDL_RenderCopy(renderer, KO_threat, NULL, &dst);
                
                SDL_RenderPresent(renderer);
                SDL_DestroyTexture(KO_threat);
                SDL_Delay(16);
                continue;
            }
            // Handle end game state (not implemented yet)
            // draw yellow screen like board background
            SDL_SetRenderDrawColor(renderer, 200, 160, 80, 255);
            SDL_RenderClear(renderer);
            
            draw_endgame_interface(renderer, font1, color, endgame_button_list, black_stone, white_stone);
            
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }

        
        if(current_state == GameState::PLAYING){
            next_state = check_game_state();
            if(next_state == GameState::LOAD_GAME){
                init_loadgame_interface();
            }
            if (next_state == GameState::END_GAME){
                endgame_time = SDL_GetTicks();
            }
            current_state = next_state;
        }
    }
    undo_button.destroy();
    redo_button.destroy();
    pass_button.destroy();
    menu_button.destroy();
    home_button1.destroy();
    home_button2.destroy();
    one_player_button.destroy();
    two_players_button.destroy();
    start_button.destroy();
    loadgame_button.destroy();
    options_button.destroy();
    exit_button.destroy();
    back_button.destroy();
    board_mode_19_button.destroy();
    board_mode_13_button.destroy();
    sound_effect_button.destroy();
    music_button.destroy();
    increase_music_button.destroy();
    decrease_music_button.destroy();
    increase_sound_effect_button.destroy();
    decrease_sound_effect_button.destroy();
    undo_button.destroy();
    redo_button.destroy();
    pass_button.destroy();
    menu_button.destroy();

    // Cleanup AI thread
    if (g_ai_thread.joinable()) {
        g_ai_thread.join();
    }

    SDL_DestroyTexture(black_stone);
    SDL_DestroyTexture(white_stone);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font1);
    TTF_CloseFont(font2);
    bgm.MixFree();
    place_stone_sound.MixFree();

    SDL_DestroyTexture(bg_rice_paper);
    SDL_DestroyTexture(bg_soft_mist);
    SDL_DestroyTexture(bg_wood);
    return 0;
}