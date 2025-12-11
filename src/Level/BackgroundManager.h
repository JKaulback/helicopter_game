#pragma once
#include "raylib.h"
#include <vector>

class BackgroundManager {
public:
    void Init();
    void Draw(float scrollDistance);

private:
    // Helper for deterministic random based on position
    float GetDeterministicRandom(int x, int seed);
    
    // Constants for generation
    const int CellSize = 40;
    const Color BackgroundCaveColor = {45, 35, 30, 255}; // Dark Brown
    const Color ForegroundCaveColor = {65, 55, 50, 255}; // Lighter Brown
};
