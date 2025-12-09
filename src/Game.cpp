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

    missiles.clear();
    spawnTimer = 0.0f;

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
            missiles.clear();
            spawnTimer = 0.0f;
        }
        return;
    }

    helicopter.Update();
    if (helicopter.HasStarted()) {
        level.Update(); // Scroll terrain
        
        // Spawn Missiles
        spawnTimer += GetFrameTime();
        if (spawnTimer > 3.0f) { // Spawn every 3 seconds for now
            spawnTimer = 0.0f;
            
            int r = GetRandomValue(0, 2);
            float targetY = level.GetCurrentGapCenter();
            Vector2 spawnPos = {(float)Constants::ScreenWidth + 50.0f, targetY};

            if (r == 0) {
                 missiles.push_back(std::make_unique<OscillatorMissile>(spawnPos));
            } else if (r == 1) {
                 missiles.push_back(std::make_unique<LooperMissile>(spawnPos));
            } else {
                 missiles.push_back(std::make_unique<SeekerMissile>(spawnPos));
            }
        }
    }

    // Update Missiles
    for (auto it = missiles.begin(); it != missiles.end(); ) {
        if ((*it)->IsActive()) {
            (*it)->Update(helicopter.GetPosition());
            
            // Check for collision with level (terrain)
            if (level.CheckCollision((*it)->GetRect())) {
                 // Explode!
                 explosions.push_back({ {(*it)->GetRect().x + 15, (*it)->GetRect().y + 5}, 0.5f });
                 it = missiles.erase(it); // Remove missile
                 continue; 
            }

            // Check for collision with player
            if (CheckCollisionRecs(helicopter.GetRect(), (*it)->GetRect())) {
                gameOver = true;
            }

            ++it;
        } else {
            it = missiles.erase(it);
        }
    }
    
    // Update Explosions
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->timer -= GetFrameTime();
        if (it->timer <= 0) {
             it = explosions.erase(it);
        } else {
             ++it;
        }
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
    
    for (auto& missile : missiles) {
        missile->Draw();
    }

    // Draw Explosions
    for (const auto& e : explosions) {
        float radius = (0.5f - e.timer) * 80.0f; // Expand
        Color col = (e.timer > 0.25f) ? ORANGE : YELLOW; // Fade color
        col.a = (unsigned char)(e.timer * 2.0f * 255.0f); // Fade alpha
        DrawCircle((int)e.position.x, (int)e.position.y, radius, col);
    }

    helicopter.Draw();

    // Draw Score
    char scoreText[50];
    sprintf(scoreText, "Distance: %d", (int)level.GetDistance());
    DrawTextEx(gameFont, scoreText, Vector2{10.0f, 10.0f}, 20, 1, BLACK);

    if (gameOver) {
        DrawRectangle(0, 0, Constants::ScreenWidth, Constants::ScreenHeight, Fade(BLACK, 0.5f));
        Vector2 textMeasure = MeasureTextEx(gameFont, "GAME OVER", 40, 2);
        DrawTextEx(gameFont, "GAME OVER", Vector2{(float)Constants::ScreenWidth/2.0f - textMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f - 20.0f}, 40, 2, RED);
        
        Vector2 subTextMeasure = MeasureTextEx(gameFont, "Press 'R' to Restart", 20, 1);
        DrawTextEx(gameFont, "Press 'R' to Restart", Vector2{(float)Constants::ScreenWidth/2.0f - subTextMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f + 30.0f}, 20, 1, DARKGRAY);
    }
    
    EndDrawing();
}
