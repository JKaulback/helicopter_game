#pragma once
#include "raylib.h"

class Explosion {
public:
    Explosion(Vector2 pos);
    
    void Update();
    void Draw() const;
    bool IsActive() const { return active; }

private:
    Vector2 position;
    float timer; // Lifetime timer
    bool active;
};
