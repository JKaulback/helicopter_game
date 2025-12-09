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

    // Calculate rotation transform
    // We want to draw pieces relative to the center of the missile.
    // Raylib's DrawRectanglePro rotates a rectangle around an origin.
    // It's easier to use rlgl for complex composite shapes, but let's stick to Raylib shapes with math.
    
    // Actually, push/pop matrix is cleaner for composite objects.
    rlPushMatrix();
    rlTranslatef(position.x + halfWidth, position.y + halfHeight, 0); // Move to center
    rlRotatef(rotation, 0, 0, 1); // Rotate

    // Draw Body (Centered)
    DrawRectangle(-halfWidth, -halfHeight, width, height, color);

    // Draw Nose Cap (Triangle at right end)
    // Points relative to center (0,0)
    Vector2 p1 = { halfWidth, -halfHeight }; // Top right corner of body
    Vector2 p2 = { halfWidth, halfHeight };  // Bottom right corner of body
    Vector2 p3 = { halfWidth + 10, 0 };      // Tip
    DrawTriangle(p1, p2, p3, GRAY);

    // Draw Engine Fire (At left end)
    // Animate fire length
    float fireLength = 10.0f + sinf(GetTime() * 20.0f) * 5.0f; 
    
    Vector2 f1 = { -halfWidth, -halfHeight + 2 }; 
    Vector2 f2 = { -halfWidth, halfHeight - 2 };
    Vector2 f3 = { -halfWidth - fireLength, 0 };
    DrawTriangle(f1, f3, f2, ORANGE); // Note order for winding

    rlPopMatrix();
}

Rectangle Missile::GetRect() const {
    return {position.x, position.y, (float)width, (float)height};
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
    
    // Calculate moving center of the loop
    // Note: timeAlive is incremented by 1/60 per frame.
    // So speed * 60 * timeAlive gives total linear pixel displacement.
    float centerX = startPos.x - (speed * 60.0f * timeAlive);
    
    float radius = loopRadius;
    float w = loopSpeed; 
    float angle = timeAlive * w; // Radians

    // Circular motion
    
    // Adjusted Center
    // If loopSpeed is negative, we need to adjust phase or center offset logic if we want consistency?
    // Actually, simple offset (-radius) works for positive speed starting at 0 angle (right side of circle).
    // If speed is negative, it starts at 0 and goes negative angle (counter-clockwise).
    // cos(0) = 1. x = center + R. So center = start - R. Same logic.
    
    centerX -= radius; 
    position.x = centerX + cosf(angle) * radius;
    
    // For Y: at t=0, sin=0, y=start. Correct.
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
    // 1. Accelerate towards player Y
    float accel = 0.05f;
    float maxVel = 2.0f; // Increased max velocity slightly to track better

    if (playerPos.y > baseY) {
        verticalVelocity += accel;
    } else {
        verticalVelocity -= accel;
    }

    // Clamp velocity
    if (verticalVelocity > maxVel) verticalVelocity = maxVel;
    if (verticalVelocity < -maxVel) verticalVelocity = -maxVel;

    // Apply velocity
    baseY += verticalVelocity;

    // 2. Add wave offset relative to baseY
    // Note: amplitude 5.0f is enough for visual effect without chaos
    float wave = sinf(timeAlive * 8.0f) * 5.0f; 
    position.y = baseY + wave;

    // Rotation
    float dx = position.x - oldPos.x;
    float dy = position.y - oldPos.y;
    rotation = atan2f(dy, dx) * (180.0f / PI);
}
