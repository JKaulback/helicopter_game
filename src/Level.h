#pragma once
#include "raylib.h"
#include <deque>

class Level {
public:
    void Init();
    void Update();
    void Draw();
    bool CheckCollision(Rectangle playerRect);
    float GetDistance() const { return distanceTraveled; }

private:
    std::deque<Rectangle> obstacles;
    Rectangle startPad;
    float distanceTraveled = 0.0f;
    int lastY = 250; 
    
    void GenerateChunk(int startX, int width);
};
