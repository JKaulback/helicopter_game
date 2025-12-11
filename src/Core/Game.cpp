#include "Game.h"
#include <ctime>
#include <cstdio>

using HeliConst = Constants::Helicopter;
using GameConst = Constants::Game;
using PhysConst = Constants::Physics;
using LevelConst = Constants::Level;

Game::Game() : isGameOver(false), victory(false) {}

Game::~Game() {
    UnloadFont(gameFont);
    CloseWindow();
}

void Game::Init() {
    SetRandomSeed((unsigned int)time(NULL));
    InitWindow(Constants::ScreenWidth, Constants::ScreenHeight, "Helicopter Game");
    SetTargetFPS(Constants::TargetFPS);
    audioManager.Init();
    
    helicopter.Init(HeliConst::StartPos); 
    level.Init();

    missiles.clear();
    explosions.clear();
    projectiles.clear();
    spawnTimer = 0.0f;
    
    currentAmmo = GameConst::MaxAmmo;
    ammoRechargeTimer = 0.0f;

    gameFont = LoadFont("assets/arial.ttf");
}

void Game::Shutdown() {
    audioManager.Shutdown();
    UnloadFont(gameFont);
    CloseWindow();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
    Shutdown();
}

void Game::Reset() {
    isGameOver = false;
    helicopter.Reset(HeliConst::StartPos);
    level.Init();
    missiles.clear();
    spawnTimer = 0.0f;
    currentAmmo = GameConst::MaxAmmo;
    ammoRechargeTimer = 0.0f;
}

void Game::Update() {
    // Music Control
    audioManager.UpdateMusic(helicopter.HasStarted() && !isGameOver);

    if (isGameOver) {
        if (IsKeyPressed(KEY_R)) {
            Reset();
        }
        return;
    }
    
    helicopter.Update();

    if (helicopter.HasStarted()) {
        level.Update(); // Update terrain
        
        // Spawn Missiles
        spawnTimer += GetFrameTime();
        if (spawnTimer > 3.0f) { // Spawn every 3 seconds
            spawnTimer = 0.0f;
            
            float targetY = level.GetCurrentGapCenter();
            Vector2 spawnPos = {(float)Constants::ScreenWidth + 50.0f, targetY};
            
            missiles.push_back(MissileFactory::CreateRandomMissile(spawnPos));
        }
    }

    // Helicopter Input - Shooting
    if (IsKeyPressed(KEY_SPACE) && currentAmmo > 0) {
        Vector2 heliPos = helicopter.GetPosition();
        // Spawn at nose (Width 40, Height 20 -> Center Right ~ 40, 10)
        projectiles.emplace_back(Vector2{heliPos.x + HeliConst::Width, heliPos.y + HeliConst::Height / 2.0f}, Vector2{PhysConst::ProjectileSpeed, 0.0f}, helicopter.IsFacingRight());
        currentAmmo--;
        audioManager.PlayShoot();
    }
    
    // Ammo Recharge
    if (currentAmmo < GameConst::MaxAmmo) {
        ammoRechargeTimer += GetFrameTime();
        if (ammoRechargeTimer >= GameConst::AmmoRechargeDelay) {
            currentAmmo++;
            ammoRechargeTimer = 0.0f;
        }
    }

    for (auto& p : projectiles) {
        if (!p.IsActive()) continue;

        p.Update();

        if (level.CheckProjectileCollision(p.GetRect())) {
            p.Deactivate();
            explosions.emplace_back(p.GetPosition());
            audioManager.PlayExplode();
        }
    }

    for (auto& m : missiles) {
        if (!m->IsActive()) continue;

        m->Update(helicopter.GetPosition());

        if (level.CheckCollision(m->GetRect())) {
            m->Destroy();
            explosions.emplace_back(Vector2{m->GetRect().x + 15, m->GetRect().y + 5});
            audioManager.PlayExplode();
        }
        
        for (auto& p : projectiles) {
            if (!p.IsActive()) continue;

            if (CheckCollisionRecs(m->GetRect(), p.GetRect())) {
                m->Destroy();
                p.Deactivate();
                
                Vector2 mid = { (m->GetRect().x + p.GetPosition().x)/2, (m->GetRect().y + p.GetPosition().y)/2 };
                explosions.emplace_back(mid);
                audioManager.PlayExplode();
                break;
            }
        }

        if (CheckCollisionRecs(helicopter.GetRect(), m->GetRect())) {
            gameOver();
        }
    }
    
    // Remove inactive projectiles
    cleanup();
        
    // Update Explosions
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->Update();
        if (!it->IsActive()) {
             it = explosions.erase(it);
        } else {
             ++it;
        }
    }
    
    if (level.CheckCollision(helicopter.GetRect())) {
        gameOver();
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    level.Draw(gameFont);
    
    for (auto& missile : missiles) {
        missile->Draw();
    }

    for (const auto& p : projectiles) {
        p.Draw();
    }
    
    // Draw Explosions
    for (const auto& e : explosions) {
        e.Draw();
    }

    helicopter.Draw();

    // Draw Control Panel
    DrawRectangle(0, 0, Constants::ScreenWidth, Constants::ControlPanelHeight, DARKGRAY);
    DrawLine(0, Constants::ControlPanelHeight, Constants::ScreenWidth, Constants::ControlPanelHeight, WHITE);

    // Draw Score
    char scoreText[50];
    sprintf(scoreText, "Distance: %d", (int)level.GetDistance());
    DrawTextEx(gameFont, scoreText, Vector2{20.0f, 15.0f}, 20, 1, WHITE);

    // Draw Ammo
    char ammoText[50];
    sprintf(ammoText, "Ammo: %d / %d", currentAmmo, GameConst::MaxAmmo);
    Color ammoColor = (currentAmmo == 0) ? RED : GREEN;
    DrawTextEx(gameFont, ammoText, Vector2{200.0f, 15.0f}, 20, 1, ammoColor);

    if (isGameOver) {
        DrawRectangle(0, 0, Constants::ScreenWidth, Constants::ScreenHeight, Fade(BLACK, 0.5f));
        Vector2 textMeasure = MeasureTextEx(gameFont, "GAME OVER", 40, 2);
        DrawTextEx(gameFont, "GAME OVER", Vector2{(float)Constants::ScreenWidth/2.0f - textMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f - 20.0f}, 40, 2, RED);
        
        Vector2 subTextMeasure = MeasureTextEx(gameFont, "Press 'R' to Restart", 20, 1);
        DrawTextEx(gameFont, "Press 'R' to Restart", Vector2{(float)Constants::ScreenWidth/2.0f - subTextMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f + 30.0f}, 20, 1, DARKGRAY);
    }
    
    EndDrawing();
}

void Game::cleanup() {
    auto pIt = projectiles.begin();
    while (pIt != projectiles.end()) {
        if (!pIt->IsActive()) {
            pIt = projectiles.erase(pIt);
        } else {
            ++pIt;
        }
    }

    // Remove inactive missiles
    auto mIt = missiles.begin();
    while (mIt != missiles.end()) {
        if (!(*mIt)->IsActive()) {
            mIt = missiles.erase(mIt);
        } else {
            ++mIt;
        }
    }
}

void Game::gameOver() {
    isGameOver = true;
    audioManager.PlayGameOver();
}
