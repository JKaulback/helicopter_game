#pragma once
#include "raylib.h"

// Base Abstract Class
class Missile {
public:
    Missile(Vector2 startPos, Color color);
    virtual ~Missile() = default;

    virtual void Update(Vector2 playerPos); // Virtual method
    void Draw();
    Rectangle GetRect() const;
    bool IsActive() const { return active; }

protected:
    Vector2 position;
    Vector2 startPos;
    bool active;
    float timeAlive;
    float rotation = 0.0f;
    Color color;
    
    // Constants
    float speed = 5.0f; // Base speed moving left
    int width = 30;
    int height = 10;
};

class OscillatorMissile : public Missile {
public:
    OscillatorMissile(Vector2 startPos);
    void Update(Vector2 playerPos) override;
};

class LooperMissile : public Missile {
public:
    LooperMissile(Vector2 startPos);
    void Update(Vector2 playerPos) override;
};

class SeekerMissile : public Missile {
public:
    SeekerMissile(Vector2 startPos);
    void Update(Vector2 playerPos) override;
};
