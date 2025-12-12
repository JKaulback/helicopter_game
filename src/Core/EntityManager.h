#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include "Missile.h"
#include "Projectile.h"
#include "Rock.h"
#include "Explosion.h"
#include "Level.h"
#include "Helicopter.h"
#include "AudioManager.h"

class EntityManager {
public:
    EntityManager();
    
    void Init();
    void Reset();
    void Update(float dt, Level& level, const Helicopter& helicopter, AudioManager& audioManager);
    void Draw();
    
    void SpawnProjectile(Vector2 pos, bool isFacingRight);
    
    // Returns true if player Collides with an entity
    bool CheckPlayerCollisions(Rectangle playerRect);

private:
    std::vector<std::unique_ptr<Missile>> missiles;
    std::vector<Projectile> projectiles;
    std::vector<Rock> rocks;
    std::vector<Explosion> explosions;

    float missileSpawnTimer = 0.0f;
    float rockSpawnTimer = 0.0f;
    
    void Cleanup();
    void SpawnEnemies(float dt, const Level& level);
    void UpdateProjectiles(Level& level, AudioManager& audioManager);
    void UpdateMissiles(const Vector2& playerPos, Level& level, AudioManager& audioManager);
    void UpdateRocks(AudioManager& audioManager);
    void UpdateExplosions();
};
