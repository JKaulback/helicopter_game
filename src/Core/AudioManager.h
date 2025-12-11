#pragma once
#include "raylib.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void Init();
    void Shutdown();
    
    // Updates music streaming and switching logic based on game state
    void UpdateMusic(bool isStarted, bool isGameOver, int delayTarget);

    void PlayShoot();
    void PlayExplode();
    void PlayGameOver();

private:
    Sound shootSound;
    Sound explodeSound;
    Sound gameOverSound;
    
    Music bgm;
    Music menu;

    int delay = 0;
};
