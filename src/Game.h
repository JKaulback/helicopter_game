#pragma once
#include "raylib.h"
#include "Constants.h"
#include "Helicopter.h"
#include "Level.h"
#include "Missile.h"
#include <vector>
#include <memory>


class Game {
public:
    Game();
    ~Game();

    void Init();
    void Run();

private:
    void Update();
    void Draw();

    Helicopter helicopter;
    Level level;
    Font gameFont;
    
    std::vector<std::unique_ptr<Missile>> missiles;
    float spawnTimer = 0.0f;

    bool gameOver;
    bool victory;
};
