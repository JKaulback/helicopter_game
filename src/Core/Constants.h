#pragma once
#include "raylib.h"

namespace Constants {
    constexpr int ScreenWidth = 1000;
    constexpr int ScreenHeight = 600;
    constexpr int ControlPanelHeight = 50;
    constexpr int TargetFPS = 60;
    constexpr float ScrollSpeed = 3.0f;
    constexpr int TerrainStep = 10;
    constexpr int GapHeight = 160;
    
    struct Game {
        static constexpr int MaxAmmo = 5;
        static constexpr float AmmoRechargeDelay = 1.5f;
    };

    struct Helicopter {
        static constexpr int Width = 40;
        static constexpr int Height = 20;
        static constexpr float Gravity = 0.2f;
        static constexpr float Thrust = 0.4f;
        static constexpr float MaxSpeed = 5.0f;
        static constexpr Vector2 StartPos = {100.0f, 330.0f};
    };

    struct Physics {
        static constexpr float ProjectileSpeed = 12.0f;
        static constexpr float ProjectileGravity = 0.08f;
        static constexpr float RockSpeed = 2.0f;
    };
    
    struct Level {
        static constexpr int MinGapHeight = 100;
        static constexpr int TargetWidth = 30;
        static constexpr int WeakSpotHeight = 30;
    };
}
