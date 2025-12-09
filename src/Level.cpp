#include "Level.h"
#include "Constants.h"

void Level::Init() {
    obstacles.clear();
    distanceTraveled = 0.0f;
    lastY = Constants::ScreenHeight / 2;

    // Initialize Start Pad
    startPad = {50, 350, 100, 20};

    // Generate safe zone (first 500 pixels)
    // We manually add floor and ceiling to create an open tunnel
    for (int x = 0; x < 500; x += Constants::TerrainStep) {
        int ceilingY = 50; 
        int floorY = 400; // Leave plenty of room for pad at 350

        // Ceiling
        obstacles.push_back({(float)x, 0, (float)Constants::TerrainStep, (float)ceilingY});
        // Floor
        obstacles.push_back({(float)x, (float)floorY, (float)Constants::TerrainStep, (float)(Constants::ScreenHeight - floorY)});
    }

    // Generate initial terrain to fill the rest of the screen plus buffer
    GenerateChunk(500, Constants::ScreenWidth + 100 - 500);
}

void Level::Update() {
    // Scroll obstacles
    for (auto& obs : obstacles) {
        obs.x -= Constants::ScrollSpeed;
    }
    
    // Scroll start pad
    startPad.x -= Constants::ScrollSpeed;
    
    distanceTraveled += Constants::ScrollSpeed;

    // Cull off-screen obstacles (assuming they are sorted by X, and front is leftmost)
    while (!obstacles.empty() && obstacles.front().x + obstacles.front().width < 0) {
        obstacles.pop_front();
    }

    // Generate new obstacles if needed
    // Check rightmost obstacle
    float rightEdge = 0;
    if (!obstacles.empty()) {
        rightEdge = obstacles.back().x + obstacles.back().width;
    }

    if (rightEdge < Constants::ScreenWidth + 50) {
        GenerateChunk((int)rightEdge, 100);
    }

}

void Level::GenerateChunk(int startX, int width) {
    for (int x = startX; x < startX + width; x += Constants::TerrainStep) {
        // Random walk
        lastY += GetRandomValue(-5, 5);
        
        // Clamp
        int minH = Constants::GapHeight / 2 + 20;
        int maxH = Constants::ScreenHeight - Constants::GapHeight / 2 - 20;
        
        if (lastY < minH) lastY = minH;
        if (lastY > maxH) lastY = maxH;

        int ceilingY = lastY - Constants::GapHeight / 2;
        int floorY = lastY + Constants::GapHeight / 2;

        if (ceilingY > 0) {
            obstacles.push_back({(float)x, 0, (float)Constants::TerrainStep, (float)ceilingY});
        }
        if (floorY < Constants::ScreenHeight) {
            obstacles.push_back({(float)x, (float)floorY, (float)Constants::TerrainStep, (float)(Constants::ScreenHeight - floorY)});
        }
    }
}

void Level::Draw() {
    DrawRectangleRec(startPad, GRAY);
    for (const auto& obs : obstacles) {
        DrawRectangleRec(obs, MAROON);
    }
}

bool Level::CheckCollision(Rectangle playerRect) {
    for (const auto& obs : obstacles) {
        if (CheckCollisionRecs(playerRect, obs)) {
            return true;
        }
    }
    // Also check screen bounds if no obstacles generated there (e.g. initial gap)
    if (playerRect.y < 0 || playerRect.y + playerRect.height > Constants::ScreenHeight) return true;
    return false;
}

