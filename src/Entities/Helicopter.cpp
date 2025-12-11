#include "Helicopter.h"
#include "Constants.h"
#include <cmath>

const int HELI_WIDTH = Constants::Helicopter::Width;
const int HELI_HEIGHT = Constants::Helicopter::Height;

static const std::vector<Shape> LeftShapes = {
    {ELLIPSE, {(float)HELI_WIDTH/2, (float)HELI_HEIGHT/2, (float)HELI_WIDTH/2, (float)HELI_HEIGHT/2}, DARKBLUE, NONE, 0.0f},
    {RECTANGLE, {(float)HELI_WIDTH - 5, 5, 25, 10}, DARKBLUE, NONE, 0.0f},
    
    {RING, {(float)HELI_WIDTH + 15, 0, 10, 10}, DARKBLUE, NONE, 0.0f, 2.0f},
    {RECTANGLE, {(float)HELI_WIDTH + 18.5f, 1, 3, 8}, BLACK, TAIL_ROTOR, 0.0f},

    {RECTANGLE, {5, -5, 30, 5}, BLACK, MAIN_ROTOR, 0.0f},
    {RECTANGLE, {18, 0, 4, 5}, BLACK, NONE, 0.0f},
    
    {RECTANGLE, {10, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f},
    {RECTANGLE, {(float)HELI_WIDTH - 12, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f},
    {RECTANGLE, {5, (float)HELI_HEIGHT/2 + 13, (float)HELI_WIDTH - 10, 2}, DARKGRAY, NONE, 0.0f},

    {ELLIPSE, {10, 5, 8, 6}, LIGHTGRAY, NONE, 0.0f}
};

static const std::vector<Shape> RightShapes = {
    {ELLIPSE, {(float)HELI_WIDTH/2, (float)HELI_HEIGHT/2, (float)HELI_WIDTH/2, (float)HELI_HEIGHT/2}, DARKBLUE, NONE, 0.0f},
    {RECTANGLE, {-20, 5, 25, 10}, DARKBLUE, NONE, 0.0f},

    {RING, {-25, 0, 10, 10}, DARKBLUE, NONE, 0.0f, 2.0f},
    {RECTANGLE, {-21.5f, 1, 3, 8}, BLACK, TAIL_ROTOR, 0.0f},

    {RECTANGLE, {5, -5, 30, 5}, BLACK, MAIN_ROTOR, 0.0f},
    {RECTANGLE, {18, 0, 4, 5}, BLACK, NONE, 0.0f},

    {RECTANGLE, {10, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f},
    {RECTANGLE, {(float)HELI_WIDTH - 12, (float)HELI_HEIGHT/2 + 5, 2, 8}, DARKGRAY, NONE, 0.0f},
    {RECTANGLE, {5, (float)HELI_HEIGHT/2 + 13, (float)HELI_WIDTH - 10, 2}, DARKGRAY, NONE, 0.0f},

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
    animationTimer = 0.0f;
}

void Helicopter::Update() {
    // Input handling
    bool inputGiven = false;

    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        velocity.y -= Constants::Helicopter::Thrust;
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
    
    if (hasStarted) animationTimer += GetFrameTime();

    if (!hasStarted) return;

    // Apply gravity
    velocity.y += Constants::Helicopter::Gravity;

    // Apply velocity
    position.x += velocity.x;
    position.y += velocity.y;
    
    // Clamp to screen bounds
    if (position.x < 0) {
        position.x = 0;
        velocity.x = 0;
    }
    if (position.x > Constants::ScreenWidth - Constants::Helicopter::Width) {
        position.x = Constants::ScreenWidth - Constants::Helicopter::Width;
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
            float speed = 20.0f;
            float scale = 1.0f;
            if (hasStarted) {
                scale = sinf(animationTimer * speed); 
            }
            
            float originalWidth = shape.rect.width;
            float newWidth = originalWidth * fabsf(scale);
            
            // Adjust x to keep centered
            shapeToDraw.rect.x += (originalWidth - newWidth) / 2.0f;
            shapeToDraw.rect.width = newWidth;
            
            shapeToDraw.Draw(position);
        } 
        else if (shape.id == TAIL_ROTOR) {
            float speed = 800.0f; 
            float angle = fmod(animationTimer * speed, 360.0f);

            shapeToDraw.rotation = angle;
            shapeToDraw.Draw(position);
        } else {
            shapeToDraw.Draw(position);
        }
    }
}

Rectangle Helicopter::GetRect() const {
    return {position.x, position.y, (float)Constants::Helicopter::Width, (float)Constants::Helicopter::Height};
}
