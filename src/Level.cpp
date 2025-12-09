#include "Level.h"

void Level::Init() {
    // Define pads
    startPad = {50, 350, 100, 20};
    endPad = {650, 350, 100, 20};

    // Define obstacles (walls, ceiling, floor)
    obstacles.push_back({0, 0, 800, 20}); // Ceiling
    obstacles.push_back({0, 430, 800, 20}); // Floor
    obstacles.push_back({0, 0, 20, 450}); // Left wall
    obstacles.push_back({780, 0, 20, 450}); // Right wall
    
    // Some middle obstacles
    obstacles.push_back({300, 100, 50, 200});
    obstacles.push_back({500, 250, 50, 200});
}

void Level::Draw() {
    // Draw pads
    DrawRectangleRec(startPad, GRAY);
    DrawRectangleRec(endPad, GREEN);

    // Draw obstacles
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
    return false;
}

bool Level::CheckLanding(Rectangle playerRect) {
    // Check if fully contained within end pad and velocity is low (velocity check not implemented here yet, just position)
    // For now, just check intersection with a tolerance
    return CheckCollisionRecs(playerRect, endPad); 
}
