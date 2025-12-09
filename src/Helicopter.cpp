#include "Helicopter.h"
#include "Constants.h"
#include <cmath>

// Define colors locally if needed or rely on raylib's defines if available globally or define here
// Raylib colors are usually macros or const structs. We can use them directly in the initializer list.

const int HELI_WIDTH = 40;
const int HELI_HEIGHT = 20;

static const std::vector<Shape> LeftShapes = {
    // Body
    {ELLIPSE, {(float)HELI_WIDTH/2, (float)HELI_HEIGHT/2, (float)HELI_WIDTH/2, (float)HELI_HEIGHT/2}, DARKBLUE, NONE, 0.0f},
    // Tail Boom (flipped)
    {RECTANGLE, {(float)HELI_WIDTH - 5, 5, 25, 10}, DARKBLUE, NONE, 0.0f},
    
    // Tail Structure
    // Blue Ring (Duct)
    {RING, {(float)HELI_WIDTH + 15, 0, 10, 10}, DARKBLUE, NONE, 0.0f, 2.0f},
    // Tail Rotor (Foreground)
    {RECTANGLE, {(float)HELI_WIDTH + 18.5f, 1, 3, 8}, BLACK, TAIL_ROTOR, 0.0f},

    // Main Rotor Blade (same relative pos)
    {RECTANGLE, {5, -5, 30, 5}, BLACK, MAIN_ROTOR, 0.0f},
    // Main Rotor Shaft (same relative pos)
    {RECTANGLE, {18, 0, 4, 5}, BLACK, NONE, 0.0f},
    
    // Landing Skis
    {RECTANGLE, {10, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f}, // Front Strut
    {RECTANGLE, {(float)HELI_WIDTH - 12, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f}, // Rear Strut
    {RECTANGLE, {5, (float)HELI_HEIGHT/2 + 13, (float)HELI_WIDTH - 10, 2}, DARKGRAY, NONE, 0.0f}, // Ski Runner

    // Cockpit Window (flipped)
    {ELLIPSE, {10, 5, 8, 6}, LIGHTGRAY, NONE, 0.0f}
};

static const std::vector<Shape> RightShapes = {
    // Body
    {ELLIPSE, {(float)HELI_WIDTH/2, (float)HELI_HEIGHT/2, (float)HELI_WIDTH/2, (float)HELI_HEIGHT/2}, DARKBLUE, NONE, 0.0f},
    // Tail Boom
    {RECTANGLE, {-20, 5, 25, 10}, DARKBLUE, NONE, 0.0f},

    // Tail Structure
    // Blue Ring (Duct)
    {RING, {-25, 0, 10, 10}, DARKBLUE, NONE, 0.0f, 2.0f},
    // Tail Rotor (Foreground)
    {RECTANGLE, {-21.5f, 1, 3, 8}, BLACK, TAIL_ROTOR, 0.0f},

    // Main Rotor Blade
    {RECTANGLE, {5, -5, 30, 5}, BLACK, MAIN_ROTOR, 0.0f},
    // Main Rotor Shaft
    {RECTANGLE, {18, 0, 4, 5}, BLACK, NONE, 0.0f},

    // Landing Skis
    {RECTANGLE, {10, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f}, // Rear Strut
    {RECTANGLE, {(float)HELI_WIDTH - 12, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f}, // Front Strut
    {RECTANGLE, {5, (float)HELI_HEIGHT/2 + 13, (float)HELI_WIDTH - 10, 2}, DARKGRAY, NONE, 0.0f}, // Ski Runner

    // Cockpit Window
    {ELLIPSE, {(float)HELI_WIDTH - 10, 5, 8, 6}, LIGHTGRAY, NONE, 0.0f}
};

void Helicopter::Init(Vector2 startPos) {
    Reset(startPos);
}

void Helicopter::Reset(Vector2 startPos) {
    position = startPos;
    velocity = {0, 0};
    hasStarted = false;
    facingRight = true;
}

void Helicopter::Update() {
    // Input handling
    bool inputGiven = false;

    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        velocity.y -= thrust;
        inputGiven = true;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        velocity.x -= 0.2f;
        inputGiven = true;
        facingRight = false;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        velocity.x += 0.2f;
        inputGiven = true;
        facingRight = true;
    }

    if (inputGiven) hasStarted = true;
    if (!hasStarted) return;

    // Apply gravity
    velocity.y += gravity;

    // Apply velocity
    position.x += velocity.x;
    position.y += velocity.y;
    
    // Clamp to screen bounds
    if (position.x < 0) {
        position.x = 0;
        velocity.x = 0;
    }
    if (position.x > Constants::ScreenWidth - width) {
        position.x = Constants::ScreenWidth - width;
        velocity.x = 0;
    }

    // Simple friction/drag
    velocity.x *= 0.98f;
    velocity.y *= 0.98f;
}

void Helicopter::Draw() {
    const std::vector<Shape>& currentParts = facingRight ? RightShapes : LeftShapes;

    for (const auto& shape : currentParts) {
        Shape shapeToDraw = shape; // Copy to allow modification for animation

        if (shape.id == MAIN_ROTOR) {
            // Animate Main Rotor (change width)
            // Use sin(time * speed) to oscillate width
            float speed = 20.0f;
            float scale = sinf((float)GetTime() * speed); 
            // We want it to look like it's spinning, so width goes from full to near 0 and back
            // Actually, a simple scale factor on width works best for 2D rotation illusion
            
            float originalWidth = shape.rect.width;
            float newWidth = originalWidth * fabsf(scale);
            
            // Adjust x to keep centered
            shapeToDraw.rect.x += (originalWidth - newWidth) / 2.0f;
            shapeToDraw.rect.width = newWidth;
            
            shapeToDraw.Draw(position);
        } 
        else if (shape.id == TAIL_ROTOR) {
            // Animate Tail Rotor using built-in rotation
            float speed = 800.0f; // Degrees per second
            float angle = fmod((float)GetTime() * speed, 360.0f);

            shapeToDraw.rotation = angle;
            shapeToDraw.Draw(position);
        } else {
            shapeToDraw.Draw(position);
        }
    }
}

Rectangle Helicopter::GetRect() const {
    return {position.x, position.y, (float)width, (float)height};
}
