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
    InitAudioDevice();
    SetTargetFPS(Constants::TargetFPS);

    // Load Audio
    // NOTE: These files are placeholders. The game will log warnings if not found but continue running.
    shootSound = LoadSound("assets/shoot.wav");
    explodeSound = LoadSound("assets/explode.wav");
    bgm = LoadMusicStream("assets/music.mp3");
    
    bgm.looping = true;
    PlayMusicStream(bgm);
    
    // Initialize game objects here
    helicopter.Init({100, 330}); // Start position
    level.Init();

    missiles.clear();
    explosions.clear();
    projectiles.clear();
    spawnTimer = 0.0f;
    
    currentAmmo = maxAmmo;
    ammoRechargeTimer = 0.0f;

    gameFont = LoadFont("assets/arial.ttf");
    gameOver = false;
    victory = false;
}

void Game::Shutdown() {
    UnloadSound(shootSound);
    UnloadSound(explodeSound);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
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

void Game::Update() {
    UpdateMusicStream(bgm);

    if (gameOver) {
        if (IsKeyPressed(KEY_R)) {
            gameOver = false;
            helicopter.Reset({100, 330});
            level.Init();
            missiles.clear();
            spawnTimer = 0.0f;
            currentAmmo = maxAmmo;
        }
        return;
    }

    if (victory) return;

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

    // Helicopter Input - Shooting
    if (IsKeyPressed(KEY_SPACE) && currentAmmo > 0) {
        Vector2 heliPos = helicopter.GetPosition();
        // Spawn at nose (Width 40, Height 20 -> Center Right ~ 40, 10)
        projectiles.push_back({ {heliPos.x + 40, heliPos.y + 10}, {12.0f, 0.0f} });
        currentAmmo--;
        PlaySound(shootSound);
    }
    
    // Ammo Recharge
    if (currentAmmo < maxAmmo) {
        ammoRechargeTimer += GetFrameTime();
        if (ammoRechargeTimer >= ammoRechargeDelay) {
            currentAmmo++;
            ammoRechargeTimer = 0.0f;
        }
    }

    // --- PHASE 1: Projectile Physics & Wall Collision ---
    for (auto& p : projectiles) {
        if (!p.active) continue;

        // Physics
        p.velocity.y += 0.08f; 
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // Wall Collision
        if (level.CheckCollision(p.GetRect())) {
            p.active = false;
            explosions.push_back({ {p.position.x, p.position.y}, 0.5f });
            PlaySound(explodeSound);
        }
        
        // Out of bounds cleanup
        if (p.position.x > Constants::ScreenWidth + 50 || p.position.y > Constants::ScreenHeight + 50) {
            p.active = false;
        }
    }

    // --- PHASE 2: Missile Physics & Wall Collision ---
    for (auto& m : missiles) {
        if (!m->IsActive()) continue;

        m->Update(helicopter.GetPosition());

        // Wall Collision
        if (m->IsActive() && level.CheckCollision(m->GetRect())) {
            m->Destroy();
            explosions.push_back({ {m->GetRect().x + 15, m->GetRect().y + 5}, 0.5f });
            PlaySound(explodeSound);
        }
    }

    // --- PHASE 3: Entity-Entity Collisions ---
    
    // Missile vs Projectile
    for (auto& m : missiles) {
        if (!m->IsActive()) continue;

        for (auto& p : projectiles) {
            if (!p.active) continue;

            if (CheckCollisionRecs(m->GetRect(), p.GetRect())) {
                // Hit!
                m->Destroy();
                p.active = false;
                
                // Explosion at midpoint
                Vector2 mid = { (m->GetRect().x + p.position.x)/2, (m->GetRect().y + p.position.y)/2 };
                explosions.push_back({ mid, 0.5f });
                PlaySound(explodeSound);
                break; // Missile handled, move to next missile
            }
        }
    }

    // Missile vs Player
    for (auto& m : missiles) {
        if (!m->IsActive()) continue;
        if (CheckCollisionRecs(helicopter.GetRect(), m->GetRect())) {
            gameOver = true;
        }
    }

    // --- PHASE 4: Cleanup ---
    
    // Remove inactive projectiles
    auto pIt = projectiles.begin();
    while (pIt != projectiles.end()) {
        if (!pIt->active) {
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

    for (const auto& p : projectiles) {
        DrawCircleV(p.position, p.radius, YELLOW);
    }
    
    // Draw Explosions
    for (const auto& e : explosions) {
        float radius = (0.5f - e.timer) * 80.0f; // Expand
        Color col = (e.timer > 0.25f) ? ORANGE : YELLOW; // Fade color
        col.a = (unsigned char)(e.timer * 2.0f * 255.0f); // Fade alpha
        DrawCircle((int)e.position.x, (int)e.position.y, radius, col);
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
    sprintf(ammoText, "Ammo: %d / %d", currentAmmo, maxAmmo);
    Color ammoColor = (currentAmmo == 0) ? RED : GREEN;
    DrawTextEx(gameFont, ammoText, Vector2{200.0f, 15.0f}, 20, 1, ammoColor);

    if (gameOver) {
        DrawRectangle(0, 0, Constants::ScreenWidth, Constants::ScreenHeight, Fade(BLACK, 0.5f));
        Vector2 textMeasure = MeasureTextEx(gameFont, "GAME OVER", 40, 2);
        DrawTextEx(gameFont, "GAME OVER", Vector2{(float)Constants::ScreenWidth/2.0f - textMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f - 20.0f}, 40, 2, RED);
        
        Vector2 subTextMeasure = MeasureTextEx(gameFont, "Press 'R' to Restart", 20, 1);
        DrawTextEx(gameFont, "Press 'R' to Restart", Vector2{(float)Constants::ScreenWidth/2.0f - subTextMeasure.x/2.0f, (float)Constants::ScreenHeight/2.0f + 30.0f}, 20, 1, DARKGRAY);
    }
    
    EndDrawing();
}
