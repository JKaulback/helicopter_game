#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void Init();
    void Shutdown();
    
    // Updates music streaming and switching logic based on game state
    void UpdateMusic(bool isLevelActive);

    void PlayShoot();
    void PlayExplode();
    void PlayGameOver();

private:
    Sound shootSound;
    Sound explodeSound;
    Sound gameOverSound;
    
    Music bgm;
    Music menu;
};
