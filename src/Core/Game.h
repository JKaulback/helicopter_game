#pragma once
#include "raylib.h"
#include "Constants.h"
#include "Helicopter.h"
#include "Level.h"
#include "Missile.h"
#include "MissileFactory.h"
#include "AudioManager.h"
#include "Projectile.h"
#include "Explosion.h"
#include <vector>
#include <memory>

class Game {
public:
    Game();
    ~Game();

    void Init();
    void Run();
    void Shutdown();

private:
    void Update();
    void Draw();
    void Reset();
    void cleanup();
    void gameOver();
    Helicopter helicopter;
    Level level;
    Font gameFont;

    // Audio
    AudioManager audioManager;
    
    std::vector<std::unique_ptr<Missile>> missiles;
    std::vector<Explosion> explosions;
    std::vector<Projectile> projectiles;

    float spawnTimer = 0.0f;

    int currentAmmo = 5;
    float ammoRechargeTimer = 0.0f;

    bool isGameOver;
    bool victory;
};
