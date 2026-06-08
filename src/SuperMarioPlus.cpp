#include "SuperMarioPlus.h"
#include <iostream>

SuperMarioPlus::SuperMarioPlus() {}
SuperMarioPlus::~SuperMarioPlus() {}

void SuperMarioPlus::Init() {
    InitWindow(800, 450, "Super Mario Plus");
    SetTargetFPS(60);
}

void SuperMarioPlus::Update() {
    // Game logic update
}

void SuperMarioPlus::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Super Mario Plus - Welcome!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
}

bool SuperMarioPlus::ShouldClose() {
    return WindowShouldClose();
}
