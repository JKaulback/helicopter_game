#pragma once
#include "raylib.h"
#include "Constants.h"

class Rock {
public:
    Rock(Vector2 pos, float radius);
    
    void Update();
    void Draw() const;
    Rectangle GetRect() const;
    bool IsActive() const { return active; }
    void Deactivate() { active = false; }
    Vector2 GetPosition() const { return position; }

private:
    Vector2 position;
    float radius;
    bool active;
};