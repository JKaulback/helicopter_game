#include "Level.h"
#include "Constants.h"


void Level::Init() {
    // Define pads
    // Start pad at [50, 350], size [100, 20]
    startPad = {50, 350, 100, 20};
    // End pad at [650, 350], size [100, 20]
    endPad = {650, 350, 100, 20};

    obstacles.clear();

    // Terrain parameters
    const int stepX = 10;            // Width of each terrain slice
    const int gapHeight = 160;       // Height of the flight path
    int currentY = 250;              // Starting center Y of the path
    
    // Generate terrain slices across the screen
    for (int x = 0; x < Constants::ScreenWidth; x += stepX) {
        // Determine whether we are near the pads
        // Start Pad zone: 50 to 150. End Pad zone: 650 to 750.
        // We add some buffer
        bool inSafeZone = (x > startPad.x - 50 && x < startPad.x + startPad.width + 50) || 
                          (x > endPad.x - 50 && x < endPad.x + endPad.width + 50);

        if (!inSafeZone) {
            // Random walk the path center
            // Change currentY by a random amount
            currentY += GetRandomValue(-5, 5);
            
            // Clamp currentY to stay within screen with some buffer
            if (currentY < gapHeight/2 + 20) currentY = gapHeight/2 + 20;
            if (currentY > Constants::ScreenHeight - gapHeight/2 - 20) currentY = Constants::ScreenHeight - gapHeight/2 - 20;
        } else {
            // In safe zones, pull the path towards a default safe height (e.g. 250)
            // or just ensure it is open. 
            // Let's drift it towards 250 smoothly if needed, or just set it:
            // But sudden jumps are bad.
            // So we just ensure obstacles don't spawn in the critical area.
            // Simplification: Do not spawn terrain in safe zones, or spawn minimal terrain.
        }

        // Calculate ceiling and floor obstacle heights
        int ceilingY = currentY - gapHeight / 2;
        int floorY = currentY + gapHeight / 2;

        // Overlay specific logic for Safe Zones to ensure pads are clear
        if (inSafeZone) {
             ceilingY = 50;  // High ceiling
             floorY = 400;   // Low floor (pads are at 350, floorY at 400 means gap is clear effectively)
             // Actually, if floorY is 400, the obstacle starts at 400 and goes down. 
             // Pad is at 350-370. So clear.
        }

        // Add Ceiling Rect
        // Top of screen is 0. Height is ceilingY.
        if (ceilingY > 0) {
            obstacles.push_back({(float)x, 0, (float)stepX, (float)ceilingY});
        }

        // Add Floor Rect
        // Starts at floorY, goes to ScreenHeight.
        if (floorY < Constants::ScreenHeight) {
            obstacles.push_back({(float)x, (float)floorY, (float)stepX, (float)(Constants::ScreenHeight - floorY)});
        }
    }

    // Add side boundaries
    obstacles.push_back({0, 0, 20, (float)Constants::ScreenHeight}); // Left wall
    obstacles.push_back({(float)Constants::ScreenWidth - 20, 0, 20, (float)Constants::ScreenHeight}); // Right wall
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
