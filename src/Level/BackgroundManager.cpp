#include "BackgroundManager.h"
#include "Constants.h"
#include <cmath>

void BackgroundManager::Init() {
    // No initialization needed yet, purely functional drawing
}

float BackgroundManager::GetDeterministicRandom(int x, int seed) {
    unsigned int n = (unsigned int)x;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731 + 789221) + 1376312589 + seed; // Standard integer hashing
    return (float)(n & 0x7fffffff) / 2147483647.0f;
}

void BackgroundManager::Draw(float scrollDistance) {
    // --- Layer 1: Background (Slower, Darker, Smaller, closer to center line) ---
    float parallaxFactor1 = 0.1f;
    float effectiveScroll1 = scrollDistance * parallaxFactor1;
    
    // Bounds
    int startCell1 = (int)((effectiveScroll1 - 100) / CellSize);
    int endCell1 = (int)((effectiveScroll1 + Constants::ScreenWidth + 100) / CellSize);
    
    for (int i = startCell1; i <= endCell1; i++) {
        // Grouping
        if (GetDeterministicRandom(i, 999) > 0.4f && GetDeterministicRandom(i, 123) > 0.3f) continue;

        float xPos = i * CellSize - effectiveScroll1;
        float heightVar = GetDeterministicRandom(i, 456);
        float widthVar = GetDeterministicRandom(i, 789);
        
        // Closer to center (Gap is 250 -> 350)
        float ceilingBase = 150.0f; 
        float floorBase = 450.0f;

        if (GetDeterministicRandom(i, 111) > 0.3f) { // Ceiling
            float h = 100.0f + heightVar * 200.0f; // Larger
            float w = 25.0f + widthVar * 40.0f;    // Wider
            DrawTriangle(
                (Vector2){xPos - w, ceilingBase - h}, 
                (Vector2){xPos, ceilingBase},      
                (Vector2){xPos + w, ceilingBase - h}, 
                BackgroundCaveColor
            );
        }
        if (GetDeterministicRandom(i, 222) > 0.3f) { // Floor
            float h = 100.0f + GetDeterministicRandom(i, 321) * 200.0f; 
            float w = 25.0f + GetDeterministicRandom(i, 654) * 40.0f;
            DrawTriangle(
                (Vector2){xPos - w, floorBase + h}, 
                (Vector2){xPos + w, floorBase + h}, 
                (Vector2){xPos, floorBase},     
                BackgroundCaveColor
            );
        }
    }

    // --- Layer 2: Foreground (Faster, Lighter, Bigger, spans full height) ---
    float parallaxFactor2 = 0.2f;
    float effectiveScroll2 = scrollDistance * parallaxFactor2;
    
    int startCell2 = (int)((effectiveScroll2 - 100) / CellSize);
    int endCell2 = (int)((effectiveScroll2 + Constants::ScreenWidth + 100) / CellSize);

    for (int i = startCell2; i <= endCell2; i++) {
        // Grouping: Dense clusters
        int groupIndex = i / 10;
        float groupVal = GetDeterministicRandom(groupIndex, 888);
        float density = (groupVal > 0.6f) ? 0.9f : 0.2f;
        
        if (GetDeterministicRandom(i, 101) > density) continue;

        float xPos = i * CellSize - effectiveScroll2;
        float heightVar = GetDeterministicRandom(i, 202);
        float widthVar = GetDeterministicRandom(i, 303);
        
        if (GetDeterministicRandom(i, 404) > 0.4f) { // Ceiling
            float h = 100.0f + heightVar * 250.0f; // Much Larger
            float w = 40.0f + widthVar * 60.0f;    // Much Wider
            DrawTriangle(
                (Vector2){xPos - w, -50}, 
                (Vector2){xPos, h - 50},      
                (Vector2){xPos + w, -50}, 
                ForegroundCaveColor
            );
        }

        if (GetDeterministicRandom(i, 505) > 0.4f) { // Floor
            float h = 100.0f + GetDeterministicRandom(i, 606) * 250.0f;
            float w = 40.0f + GetDeterministicRandom(i, 707) * 60.0f;
            float baseY = (float)Constants::ScreenHeight + 50.0f;
            DrawTriangle(
                (Vector2){xPos - w, baseY}, 
                (Vector2){xPos + w, baseY}, 
                (Vector2){xPos, baseY - h},     
                ForegroundCaveColor
            );
        }
    }
}
