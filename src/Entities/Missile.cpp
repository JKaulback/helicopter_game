#include "Missile.h"
#include <cmath>
#include "Constants.h"
#include "raymath.h"
#include "rlgl.h"

// --- Base Missile ---
Missile::Missile(Vector2 startPos, Color color) 
    : position(startPos), startPos(startPos), active(true), timeAlive(0.0f), color(color) {}

void Missile::Update(Vector2 playerPos) {
    timeAlive += 1.0f / 60.0f; 

    // Deactivate if off screen
    if (position.x < -50) active = false;
}

void Missile::Draw() {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // Body (Centered at 0,0 for local rotation)
    Rectangle bodyRect = { 0, 0, (float)width, (float)height };
    Vector2 bodyOrigin = { halfWidth, halfHeight }; 

    // Draw missile based on rotation
    rlPushMatrix();
    rlTranslatef(position.x + halfWidth, position.y + halfHeight, 0); // Move to center
    rlRotatef(rotation, 0, 0, 1); // Rotate

    // Draw Body
    DrawRectangle(-halfWidth, -halfHeight, width, height, color);

    // Draw Nose Cap (Triangle at right end)
    Vector2 p1 = { halfWidth, -halfHeight }; // Top right corner of body
    Vector2 p2 = { halfWidth, halfHeight };  // Bottom right corner of body
    Vector2 p3 = { halfWidth + 10, 0 };      // Tip
    DrawTriangle(p1, p2, p3, GRAY);

    // Draw Engine Fire (At left end)
    float fireLength = 10.0f + sinf(GetTime() * 20.0f) * 5.0f; 
    
    Vector2 f1 = { -halfWidth, -halfHeight + 2 }; 
    Vector2 f2 = { -halfWidth, halfHeight - 2 };
    Vector2 f3 = { -halfWidth - fireLength, 0 };
    DrawTriangle(f1, f3, f2, ORANGE);

    rlPopMatrix();
}

Rectangle Missile::GetRect() const {
    // Include the nose cap (10px) in the collision box
    return {position.x, position.y, (float)width + 10.0f, (float)height};
}

// --- Oscillator Missile ---
OscillatorMissile::OscillatorMissile(Vector2 startPos) : Missile(startPos, RED) {
    amplitude = (float)GetRandomValue(40, 90);
    frequency = (float)GetRandomValue(20, 50) / 10.0f;
}

void OscillatorMissile::Update(Vector2 playerPos) {
    Vector2 oldPos = position;
    
    Missile::Update(playerPos); // Base updates timeAlive
    if (!active) return;

    position.x -= speed;
    
    // Sine wave motion
    float wave = sinf(timeAlive * frequency) * amplitude;
    position.y = startPos.y + wave;

    // Rotation
    float dx = position.x - oldPos.x;
    float dy = position.y - oldPos.y;
    rotation = atan2f(dy, dx) * (180.0f / PI);
}

// --- LooperMissile ---
LooperMissile::LooperMissile(Vector2 startPos) : Missile(startPos, PURPLE) {
    loopRadius = (float)GetRandomValue(40, 70);
    loopSpeed = (float)GetRandomValue(50, 80) / 10.0f;
    // 50% chance to flip loop direction
    if (GetRandomValue(0, 1) == 0) loopSpeed *= -1;
}

void LooperMissile::Update(Vector2 playerPos) {
    Vector2 oldPos = position;

    Missile::Update(playerPos);
    if (!active) return;

    // To loop back, the circular velocity must exceed linear velocity.
    float centerX = startPos.x - (speed * 60.0f * timeAlive);
    float radius = loopRadius;
    float w = loopSpeed; 
    float angle = timeAlive * w;

    // Circular motion
    centerX -= radius; 
    position.x = centerX + cosf(angle) * radius;
    position.y = startPos.y + sinf(angle) * radius;

    // Rotation
    float dx = position.x - oldPos.x;
    float dy = position.y - oldPos.y;
    rotation = atan2f(dy, dx) * (180.0f / PI);
}

// --- Seeker Missile ---
SeekerMissile::SeekerMissile(Vector2 startPos) : Missile(startPos, ORANGE), baseY(startPos.y), verticalVelocity(0.0f) {}

void SeekerMissile::Update(Vector2 playerPos) {
    Vector2 oldPos = position;

    Missile::Update(playerPos);
    if (!active) return;

    position.x -= speed;

    // Seeker logic (Smooth with Inertia)
    float accel = 0.05f;
    float maxVel = 2.0f;

    if (playerPos.y > baseY) {
        verticalVelocity += accel;
    } else {
        verticalVelocity -= accel;
    }

    if (verticalVelocity > maxVel) verticalVelocity = maxVel;
    if (verticalVelocity < -maxVel) verticalVelocity = -maxVel;

    baseY += verticalVelocity;

    float wave = sinf(timeAlive * 8.0f) * 5.0f; 
    position.y = baseY + wave;

    float dx = position.x - oldPos.x;
    float dy = position.y - oldPos.y;
    rotation = atan2f(dy, dx) * (180.0f / PI);
}
