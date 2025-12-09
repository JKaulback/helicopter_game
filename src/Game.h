#pragma once
#include "raylib.h"
#include "Constants.h"
#include "Helicopter.h"
#include "Level.h"
#include "Missile.h"
#include <vector>
#include <memory>


class Game {
public:
    Game();
    ~Game();

    void Init();
    void Run();

private:
    void Update();
    void Draw();

    Helicopter helicopter;
    Level level;
    Font gameFont;
    
    std::vector<std::unique_ptr<Missile>> missiles;
    
    struct Explosion {
        Vector2 position;
        float timer; // 0.0 to 1.0 (lifetime)
    };
    std::vector<Explosion> explosions;

    struct Projectile {
        Vector2 position;
        Vector2 velocity;
        bool active = true;
        float radius = 5.0f;
        Rectangle GetRect() const { return {position.x - radius, position.y - radius, radius*2, radius*2}; }
    };
    std::vector<Projectile> projectiles;

    float spawnTimer = 0.0f;
    
    // Ammo System
    int maxAmmo = 5;
    int currentAmmo = 5;
    float ammoRechargeTimer = 0.0f;
    const float ammoRechargeDelay = 1.5f;

    bool gameOver;
    bool victory;
};
