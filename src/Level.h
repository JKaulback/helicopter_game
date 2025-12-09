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
    float GetCurrentGapCenter() const { return lastY; }

private:
    std::deque<Rectangle> obstacles;
    Rectangle startPad;
    float distanceTraveled = 0.0f;
    float lastY = 250.0f; 
    
    float targetY = 250.0f;
    int stepsToTarget = 0;
    
    float currentGapHeight = 300.0f;

    void GenerateChunk(int startX, int width);
};
