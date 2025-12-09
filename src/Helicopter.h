#pragma once
#include "raylib.h"
#include "Shape.h"
#include <vector>

class Helicopter {
public:
    void Init(Vector2 startPos);
    void Update();
    void Draw();
    void Reset(Vector2 startPos);
    Rectangle GetRect() const;
    bool HasStarted() const { return hasStarted; }
    Vector2 GetPosition() const { return position; }

private:
    Vector2 position;
    Vector2 velocity;
    const float gravity = 0.2f;
    const float thrust = 0.4f;
    const float maxSpeed = 5.0f;
    const int width = 40;
    const int height = 20;
    bool hasStarted;
    bool facingRight;
};
