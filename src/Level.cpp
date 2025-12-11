#include "Level.h"
#include "Constants.h"
#include <cmath>

void Level::Init() {
    obstacles.clear();
    targets.clear();
    distanceTraveled = 0.0f;
    lastY = (Constants::ScreenHeight + Constants::ControlPanelHeight) / 2.0f;
    targetY = lastY;
    stepsToTarget = 0;
    currentGapHeight = 300.0f; // Start wide

    // Initialize Start Pad
    startPad = {50, 350, 100, 20};

    // Generate safe zone (first 500 pixels)
    for (int x = 0; x < 500; x += Constants::TerrainStep) {
        int ceilingY = Constants::ControlPanelHeight + 50; 
        int floorY = 400; 

        obstacles.push_back({(float)x, (float)Constants::ControlPanelHeight, (float)Constants::TerrainStep, (float)(ceilingY - Constants::ControlPanelHeight)});
        obstacles.push_back({(float)x, (float)floorY, (float)Constants::TerrainStep, (float)(Constants::ScreenHeight - floorY)});
    }

    // Generate initial terrain
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

    // Scroll targets
    for (auto& target : targets) {
        target.rect.x -= Constants::ScrollSpeed;
        target.weakSpot.x -= Constants::ScrollSpeed;
    }
    
    // Cull off-screen targets
    while (!targets.empty() && targets.front().rect.x + targets.front().rect.width < 0) {
        targets.pop_front();
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
        
        // Narrow the gap
        if (currentGapHeight > 100.0f) {
            currentGapHeight -= 0.05f; // Shrink slowly
        }

        // Target Logic
        stepsToTarget--;
        if (stepsToTarget <= 0) {
            // Pick new target based on CURRENT gap height, respecting Control Panel
            int minSafe = Constants::ControlPanelHeight + (int)(currentGapHeight / 2.0f) + 50;
            int maxSafe = Constants::ScreenHeight - (int)(currentGapHeight / 2.0f) - 50;
            
            // Ensure bounds are valid (avoid crossing)
            if (minSafe > maxSafe) {
                minSafe = (Constants::ScreenHeight + Constants::ControlPanelHeight) / 2 - 20;
                maxSafe = (Constants::ScreenHeight + Constants::ControlPanelHeight) / 2 + 20;
            }

            targetY = (float)GetRandomValue(minSafe, maxSafe);
            stepsToTarget = GetRandomValue(30, 80);
        }

        // Move towards target (Smoothing)
        float diff = targetY - lastY;
        float move = 0.0f;
        if (std::abs(diff) > 1.0f) {
             move = (diff > 0) ? 1.0f : -1.0f;
        } else {
             move = diff; // Snap to small diffs
        }
        
        // Very occasional noise for slight organic feel, but mostly smooth
        if (GetRandomValue(0, 10) == 0) {
            move += (float)GetRandomValue(-1, 1) * 0.5f;
        }

        lastY += move;
        
        // Clamp
        float minH = Constants::ControlPanelHeight + currentGapHeight / 2.0f + 20.0f;
        float maxH = Constants::ScreenHeight - currentGapHeight / 2.0f - 20.0f;
        
        if (lastY < minH) lastY = minH;
        if (lastY > maxH) lastY = maxH;

        int ceilingY = (int)(lastY - currentGapHeight / 2.0f);
        int floorY = (int)(lastY + currentGapHeight / 2.0f);

        if (ceilingY > Constants::ControlPanelHeight) {
            obstacles.push_back({(float)x, (float)Constants::ControlPanelHeight, (float)Constants::TerrainStep, (float)(ceilingY - Constants::ControlPanelHeight)});
        }
        if (floorY < Constants::ScreenHeight) {
            obstacles.push_back({(float)x, (float)floorY, (float)Constants::TerrainStep, (float)(Constants::ScreenHeight - floorY)});
        }

        if (GetRandomValue(0, 100) < 2) { // 2% chance per step? steps are small.
             // ensure distance from last target
             bool canSpawn = true;
             if (!targets.empty()) {
                 if (x - targets.back().rect.x < 400) canSpawn = false; 
             }
             
             if (canSpawn && (floorY - ceilingY) > 60) { // Enough space
                 float tWidth = 30.0f;
                 float tHeight = (float)(floorY - ceilingY);
                 float tX = (float)x;
                 float tY = (float)ceilingY;
                 
                 // Weak spot
                 float wHeight = 30.0f;
                 float wY = (float)GetRandomValue((int)tY, (int)(tY + tHeight - wHeight));
                 
                 targets.push_back({{tX, tY, tWidth, tHeight}, {tX, wY, tWidth, wHeight}, true});
             }
        }
    }
}

void Level::Draw() {
    DrawRectangleRec(startPad, GRAY);
    for (const auto& obs : obstacles) {
        DrawRectangleRec(obs, MAROON);
    }
    
    for (const auto& target : targets) {
        if (!target.active) continue;
        DrawRectangleRec(target.rect, GRAY);
        DrawRectangleRec(target.weakSpot, GREEN);
    }
}

bool Level::CheckCollision(Rectangle playerRect) {

    for (const auto& obs : obstacles) {
        if (CheckCollisionRecs(playerRect, obs)) {
            return true;
        }
    }
    
    for (const auto& target : targets) {
        if (target.active && CheckCollisionRecs(playerRect, target.rect)) {
            return true;
        }
    }

    // Also check screen bounds if no obstacles generated there (e.g. initial gap)
    if (playerRect.y + playerRect.height > Constants::ScreenHeight) return true;
    return false;
}

bool Level::CheckProjectileCollision(Rectangle projRect) {
    // Check Obstacles
    for (const auto& obs : obstacles) {
        if (CheckCollisionRecs(projRect, obs)) return true;
    }

    // Check Targets
    for (auto& target : targets) {
        if (!target.active) continue;
        if (CheckCollisionRecs(projRect, target.rect)) {
            // Check Weak Spot
            if (CheckCollisionRecs(projRect, target.weakSpot)) {
                target.active = false; // Destroy!
                return true; 
            }
            return true; // Wall hit
        }
    }
    
    // Check Bounds
    if (projRect.y + projRect.height > Constants::ScreenHeight) return true;
    
    return false;
}

