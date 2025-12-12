#include "EntityManager.h"
#include "Constants.h"
#include "MissileFactory.h"
#include <algorithm>

using PhysConst = Constants::Physics;

EntityManager::EntityManager() {}

void EntityManager::Init() {
    Reset();
}

void EntityManager::Reset() {
    missiles.clear();
    projectiles.clear();
    rocks.clear();
    explosions.clear();
    missileSpawnTimer = 0.0f;
    rockSpawnTimer = 0.0f;
}

void EntityManager::SpawnProjectile(Vector2 pos, bool isFacingRight) {
    projectiles.emplace_back(pos, Vector2{PhysConst::ProjectileSpeed, 0.0f}, isFacingRight);
}

void EntityManager::Update(float dt, Level& level, const Helicopter& helicopter, AudioManager& audioManager) {
    SpawnEnemies(dt, level);
    
    UpdateProjectiles(level, audioManager);
    UpdateMissiles(helicopter.GetPosition(), level, audioManager);
    UpdateRocks(audioManager);
    UpdateExplosions();
    
    Cleanup();
}

void EntityManager::SpawnEnemies(float dt, const Level& level) {
    if (level.GetDistance() < 1500.0f) return; // Warmup

    // Missiles
    missileSpawnTimer += dt;
    if (missileSpawnTimer > 3.0f) {
        missileSpawnTimer = 0.0f;
        float targetY = level.GetCurrentGapCenter();
        Vector2 spawnPos = {(float)Constants::ScreenWidth + 50.0f, targetY};
        missiles.push_back(MissileFactory::CreateRandomMissile(spawnPos));
    }

    // Rocks
    if (level.GetDistance() > 2500.0f) {
        rockSpawnTimer += dt;
        if (rockSpawnTimer > 5.0f) {
            rockSpawnTimer = 0.0f;
            float targetY = level.GetCurrentGapCenter();
            Vector2 spawnPos = {0, targetY};
            rocks.push_back(Rock(spawnPos, 15.0f));
        }
    }
}

void EntityManager::UpdateProjectiles(Level& level, AudioManager& audioManager) {
    for (auto& p : projectiles) {
        if (!p.IsActive()) continue;
        p.Update();

        if (level.CheckProjectileCollision(p.GetRect())) {
            p.Deactivate();
            explosions.emplace_back(p.GetPosition());
            audioManager.PlayExplode();
        }
    }
}

void EntityManager::UpdateMissiles(const Vector2& playerPos, Level& level, AudioManager& audioManager) {
    for (auto& m : missiles) {
        if (!m->IsActive()) continue;

        m->Update(playerPos);

        // Wall/Obstacle Collision
        if (level.CheckCollision(m->GetRect())) {
            m->Deactivate();
            explosions.emplace_back(Vector2{m->GetRect().x + 15, m->GetRect().y + 5});
            audioManager.PlayExplode();
        }
        
        // Projectile Collision
        for (auto& p : projectiles) {
            if (!p.IsActive()) continue;
            if (CheckCollisionRecs(m->GetRect(), p.GetRect())) {
                m->Deactivate();
                p.Deactivate();
                Vector2 mid = { (m->GetRect().x + p.GetPosition().x)/2, (m->GetRect().y + p.GetPosition().y)/2 };
                explosions.emplace_back(mid);
                audioManager.PlayExplode();
                break;
            }
        }
    }
}

void EntityManager::UpdateRocks(AudioManager& audioManager) {
    for (auto& r : rocks) {
        if (!r.IsActive()) continue;
        r.Update();

        // Projectile Collision
        for (auto& p : projectiles) {
            if (!p.IsActive()) continue;
            if (CheckCollisionRecs(r.GetRect(), p.GetRect())) {
                r.Deactivate();
                p.Deactivate();
                Vector2 mid = { (r.GetRect().x + p.GetPosition().x)/2, (r.GetRect().y + p.GetPosition().y)/2 };
                explosions.emplace_back(mid);
                audioManager.PlayExplode();
                break;
            }
        }
    }
}

void EntityManager::UpdateExplosions() {
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->Update();
        if (!it->IsActive()) {
             it = explosions.erase(it);
        } else {
             ++it;
        }
    }
}

bool EntityManager::CheckPlayerCollisions(Rectangle playerRect) {
    // Missiles
    for (const auto& m : missiles) {
        if (m->IsActive() && CheckCollisionRecs(playerRect, m->GetRect())) {
            return true;
        }
    }
    
    // Rocks
    for (const auto& r : rocks) {
        if (r.IsActive() && CheckCollisionRecs(playerRect, r.GetRect())) {
            return true;
        }
    }
    
    return false;
}

void EntityManager::Cleanup() {
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), 
        [](const auto& p) { return !p.IsActive(); }), projectiles.end());

    missiles.erase(std::remove_if(missiles.begin(), missiles.end(), 
        [](const auto& m) { return !m->IsActive(); }), missiles.end());

    rocks.erase(std::remove_if(rocks.begin(), rocks.end(), 
        [](const auto& r) { return !r.IsActive(); }), rocks.end());
}

void EntityManager::Draw() {
    for (auto& missile : missiles) missile->Draw();
    for (const auto& p : projectiles) p.Draw();
    for (const auto& e : explosions) e.Draw();
    for (const auto& r : rocks) r.Draw();
}
