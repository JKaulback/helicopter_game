#pragma once
#include "raylib.h"
#include "Helicopter.h"
#include "Level.h"
#include "Constants.h"


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
    bool gameOver;
    bool victory;
    Font gameFont;
};
