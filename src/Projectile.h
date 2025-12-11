#pragma once
#include "raylib.h"
#include "Constants.h"

class Projectile {
public:
    Projectile(Vector2 startPos, Vector2 initialVelocity);
    
    void Update();
    void Draw() const;
    Rectangle GetRect() const;
    bool IsActive() const { return active; }
    void Deactivate() { active = false; }
    Vector2 GetPosition() const { return position; }

private:
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
};
