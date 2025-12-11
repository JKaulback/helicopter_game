#include "Level.h"
#include "Constants.h"
#include <cmath>

void Level::Init() {
    obstacles.clear();
    walls.clear();
    triangleObstacles.clear();
    levelTexts.clear();
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

    // Add Tutorial Text
    levelTexts.push_back({{200.0f, 200.0f}, "W / UP to Fly", 20, LIGHTGRAY});
    levelTexts.push_back({{200.0f, 230.0f}, "A / LEFT to Move Left", 20, LIGHTGRAY});
    levelTexts.push_back({{200.0f, 260.0f}, "D / RIGHT to Move Right", 20, LIGHTGRAY});
    levelTexts.push_back({{200.0f, 290.0f}, "SPACE to Shoot", 20, LIGHTGRAY});
    levelTexts.push_back({{200.0f, 320.0f}, "Avoid Obstacles!", 20, MAROON});

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
    
    // Scroll texts
    for (auto& txt : levelTexts) {
        txt.position.x -= Constants::ScrollSpeed;
    }

    // Remove off-screen texts
    if (!levelTexts.empty() && levelTexts.front().position.x < -300) {
        levelTexts.pop_front();
    }
    
    distanceTraveled += Constants::ScrollSpeed;

    while (!obstacles.empty() && obstacles.front().x + obstacles.front().width < 0) {
        obstacles.pop_front();
    }

    // Scroll walls
    for (auto& wall : walls) {
        wall.rect.x -= Constants::ScrollSpeed;
        wall.weakSpot.x -= Constants::ScrollSpeed;
    }
    
    // Cull off-screen walls
    while (!walls.empty() && walls.front().rect.x + walls.front().rect.width < 0) {
        walls.pop_front();
    }
    
    // Scroll Triangles
    for (auto& tri : triangleObstacles) {
        tri.p1.x -= Constants::ScrollSpeed;
        tri.p2.x -= Constants::ScrollSpeed;
        tri.p3.x -= Constants::ScrollSpeed;
    }
    
    // Cull off-screen triangles
    while (!triangleObstacles.empty() && triangleObstacles.front().p3.x < 0) {
        triangleObstacles.pop_front();
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
        if (currentGapHeight > Constants::Level::MinGapHeight) {
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
        
        // Random Stalactites/Stalagmites (Obstacles)
        if (GetRandomValue(0, 25) == 0) {
            bool onCeiling = GetRandomValue(0, 1) == 0;
            float triH = (float)GetRandomValue(30, 80);
            float triW = (float)GetRandomValue(15, 30);
            
            float centerX = (float)x + Constants::TerrainStep / 2.0f;
            
            if (onCeiling) {
                 float base = (float)(ceilingY) - 15;
                 triangleObstacles.push_back({
                     {centerX - triW, base},
                     {centerX, base + triH},
                     {centerX + triW, base},
                     true
                 });
            } else {
                 float base = (float)(floorY) + 15;
                 triangleObstacles.push_back({
                     {centerX - triW, base},
                     {centerX + triW, base},
                     {centerX, base - triH},
                     true
                 });
            }
        }
        
        // Spawn walls (2% chance per step)
        if (distanceTraveled > 500 && GetRandomValue(0, 100) < 2) { 
             // ensure distance from last wall
             bool canSpawn = true;
             if (!walls.empty()) {
                 if (x - walls.back().rect.x < 400) canSpawn = false; 
             }
             
             if (canSpawn && (floorY - ceilingY) > Constants::Level::MinGapHeight * 0.6f) {
                 float tWidth = (float)Constants::Level::TargetWidth;
                 float tHeight = (float)Constants::ScreenHeight;
                 float tX = (float)x;
                 float tY = 0.0f; 
                 
                 float wHeight = (float)Constants::Level::WeakSpotHeight;
                 float gapTop = ceilingY + 20;
                 float gapHeight = (float)(floorY - ceilingY) - 40;
                 
                 float wY = (float)GetRandomValue((int)gapTop, (int)(gapTop + gapHeight - wHeight));
                 
                 walls.push_back({{tX, tY, tWidth, tHeight}, {tX, wY, tWidth, wHeight}, true});
             }
        }
    }
}

void Level::Draw(const Font& font) {
    DrawRectangleRec(startPad, GRAY);
    
    for (const auto& txt : levelTexts) {
        DrawTextEx(font, txt.text, txt.position, (float)txt.fontSize, 1.0f, txt.color);
    }
    
    for (const auto& wall : walls) {
        if (!wall.active) continue;
        DrawRectangleRec(wall.rect, LIGHTGRAY);
        DrawRectangleRec(wall.weakSpot, GREEN);
    }

    for (const auto& obs : obstacles) {
        DrawRectangleRec(obs, BROWN);
    }
    
    for (const auto& tri : triangleObstacles) {
        DrawTriangle(tri.p1, tri.p2, tri.p3, BROWN);
    }
}

bool Level::CheckCollision(Rectangle playerRect) {

    for (const auto& obs : obstacles) {
        if (CheckCollisionRecs(playerRect, obs)) {
            return true;
        }
    }
    
    for (const auto& tri : triangleObstacles) {
        // Approximate collision (check player corners against triangle)
        Vector2 tl = {playerRect.x, playerRect.y};
        Vector2 tr = {playerRect.x + playerRect.width, playerRect.y};
        Vector2 bl = {playerRect.x, playerRect.y + playerRect.height};
        Vector2 br = {playerRect.x + playerRect.width, playerRect.y + playerRect.height};
        
        if (CheckCollisionPointTriangle(tl, tri.p1, tri.p2, tri.p3) ||
            CheckCollisionPointTriangle(tr, tri.p1, tri.p2, tri.p3) ||
            CheckCollisionPointTriangle(bl, tri.p1, tri.p2, tri.p3) ||
            CheckCollisionPointTriangle(br, tri.p1, tri.p2, tri.p3)) {
            return true;        
        }
    }
    
    for (const auto& wall : walls) {
        if (wall.active && CheckCollisionRecs(playerRect, wall.rect)) {
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

    // Check Walls
    for (auto& wall : walls) {
        if (!wall.active) continue;
        if (CheckCollisionRecs(projRect, wall.rect)) {
            // Check Weak Spot
            if (CheckCollisionRecs(projRect, wall.weakSpot)) {
                wall.active = false; // Destroy!
                return true; 
            }
            return true; // Wall hit
        }
    }
    
    // Check Bounds
    if (projRect.y + projRect.height > Constants::ScreenHeight) return true;
    
    return false;
}

