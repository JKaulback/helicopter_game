# Helicopter Game

A 2D side-scrolling helicopter game built with C++ and Raylib. Fly your helicopter, avoid procedurally generated terrain, shoot down diverse enemies, and climb the leaderboard!

## Features

### 🚁 Gameplay
*   **Physics-based Flight**: Master the gravity and thrust mechanics to navigate tight spaces.
*   **Procedural Generation**: Infinite terrain with dynamic obstacles, moving walls, and destructible barriers.
*   **Combat System**:
    *   **Ammo Management**: Limited ammo that recharges over time.
    *   **Projectiles**: Shoot to destroy walls and incoming enemies.

### 👾 Enemies
*   **Rocks**: Destructible space debris drifting through the cavern.
*   **Missiles**:
    *   **Standard**: Straight-flying rockets.
    *   **Oscillator**: Missiles that move in a distinct wave pattern.
    *   **Looper**: Tricky missiles that perform loops to throw off your aim.
    *   **Seeker**: Advanced missiles that adjust their vertical trajectory to track you.

### 🏆 Leaderboard
*   **Local High Scores**: Top 5 high scores are saved locally to `leaderboard.csv`.
*   **Name Entry**: Enter your name upon achieving a high score.

## Controls

*   **Fly Up**: `W` or `Up Arrow`
*   **Move Left**: `A` or `Left Arrow`
*   **Move Right**: `D` or `Right Arrow`
*   **Shoot**: `Space`
*   **Restart**: `R` (On Game Over screen)
*   **Confirm Name**: `Enter` (On High Score screen)

## Building the Project

This project uses CMake to handle dependencies (Raylib).

1.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```
2.  Generate build files:
    ```bash
    cmake ..
    ```
3.  Build the project:
    ```bash
    cmake --build .
    ```
4.  Run the game:
    ```bash
    ./HelicopterGame.exe
    ```

## Requirements
*   C++17 compatible compiler
*   CMake 3.14+
*   Internet connection (to fetch Raylib during the first build)

## Credits

*   **Programming**: Created with C++ and Raylib.
*   **Music**:
    *   **Rob0neMusic**: [Background Track](https://youtu.be/ApdlmRlLXAg)
    *   **FesliyanStudios**: [Menu Music](https://www.youtube.com/watch?v=NjpjKDqKwfo)
*   **Sound Effects**:
    *   **freesound_community**: [Pixabay](https://pixabay.com//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=6435)
*   **Library**: [Raylib](https://www.raylib.com/)
