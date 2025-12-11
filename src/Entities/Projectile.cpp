#include "Projectile.h"

using PhysConst = Constants::Physics;

Projectile::Projectile(Vector2 startPos, Vector2 initialVelocity, bool isMovingRight) 
    : position(startPos), velocity(initialVelocity), active(true), radius(5.0f), isMovingRight(isMovingRight) {
}

void Projectile::Update() {
    if (!active) return;

    // Apply gravity
    velocity.y += PhysConst::ProjectileGravity; 
    
    // Apply velocity
    if (isMovingRight) {
        position.x += velocity.x;
    } else {
        position.x -= velocity.x;
    }
    position.y += velocity.y;

    // Out of bounds check (simple)
    if (position.x > Constants::ScreenWidth + 50 || position.y > Constants::ScreenHeight + 50) {
        active = false;
    }
}

void Projectile::Draw() const {
    if (active) {
        DrawCircleV(position, radius, YELLOW);
    }
}

Rectangle Projectile::GetRect() const {
    return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
}
