#pragma once
#include "raylib.h"
#include <vector>

class Level {
public:
    void Init();
    void Draw();
    bool CheckCollision(Rectangle playerRect);
    bool CheckLanding(Rectangle playerRect);

private:
    std::vector<Rectangle> obstacles;
    Rectangle startPad;
    Rectangle endPad;
};
