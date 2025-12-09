#include "Game.h"
#include <ctime>
#include <cstdio>

Game::Game() : gameOver(false), victory(false) {}

Game::~Game() {
    UnloadFont(gameFont);
    CloseWindow();
}

void Game::Init() {
    SetRandomSeed((unsigned int)time(NULL));
    InitWindow(Constants::ScreenWidth, Constants::ScreenHeight, "Helicopter Game");
    SetTargetFPS(Constants::TargetFPS);
    
    // Initialize game objects here
    helicopter.Init({100, 330}); // Start position
    level.Init();

    gameFont = LoadFont("assets/arial.ttf");
}

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
}

void Game::Update() {
    if (gameOver) {
        if (IsKeyPressed(KEY_R)) {
            gameOver = false;
            helicopter.Reset({100, 330});
            level.Init();
        }
        return;
    }

    helicopter.Update();
    if (helicopter.HasStarted()) {
        level.Update(); // Scroll terrain
    }
    
    // Check collisions
    if (level.CheckCollision(helicopter.GetRect())) {
        gameOver = true;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    level.Draw();
    helicopter.Draw();

    // Draw Score
    char scoreText[50];
    sprintf(scoreText, "Distance: %d", (int)level.GetDistance());
    DrawTextEx(gameFont, scoreText, {10, 10}, 20, 1, BLACK);

    if (gameOver) {
        DrawRectangle(0, 0, Constants::ScreenWidth, Constants::ScreenHeight, Fade(BLACK, 0.5f));
        Vector2 textMeasure = MeasureTextEx(gameFont, "GAME OVER", 40, 2);
        DrawTextEx(gameFont, "GAME OVER", {Constants::ScreenWidth/2 - textMeasure.x/2, Constants::ScreenHeight/2 - 20}, 40, 2, RED);
        
        Vector2 subTextMeasure = MeasureTextEx(gameFont, "Press 'R' to Restart", 20, 1);
        DrawTextEx(gameFont, "Press 'R' to Restart", {Constants::ScreenWidth/2 - subTextMeasure.x/2, Constants::ScreenHeight/2 + 30}, 20, 1, DARKGRAY);
    }

    EndDrawing();
}
