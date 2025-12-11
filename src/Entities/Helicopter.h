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
    bool IsFacingRight() const { return facingRight; }

private:
    Vector2 position;
    Vector2 velocity;
    bool hasStarted;
    bool facingRight;
    float animationTimer;
};
