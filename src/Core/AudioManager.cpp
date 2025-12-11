#include "AudioManager.h"

AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {
}

void AudioManager::Init() {
    InitAudioDevice();

    // Load Audio
    // NOTE: These files are placeholders. Raylib logs warnings if not found.
    shootSound = LoadSound("assets/shoot.wav");
    explodeSound = LoadSound("assets/explode.wav");
    gameOverSound = LoadSound("assets/gameover.wav");
    
    bgm = LoadMusicStream("assets/music.mp3");
    menu = LoadMusicStream("assets/menu.mp3");
    
    bgm.looping = true;
    menu.looping = true;

    delay = 0;
}

void AudioManager::Shutdown() {
    UnloadSound(shootSound);
    UnloadSound(explodeSound);
    UnloadSound(gameOverSound);
    
    UnloadMusicStream(bgm);
    UnloadMusicStream(menu);
    
    CloseAudioDevice();
}

void AudioManager::UpdateMusic(bool isStarted, bool isGameOver, int delayTarget) {
    // While the game is started and not game over, play bgm
    if (isStarted && !isGameOver) {
        if (IsMusicStreamPlaying(menu)) StopMusicStream(menu);
        if (!IsMusicStreamPlaying(bgm)) PlayMusicStream(bgm);
        UpdateMusicStream(bgm);
        delay = 0;
    // If the game is over, stop bgm and delay menu music
    } else if (isStarted && isGameOver) {
        if (IsMusicStreamPlaying(bgm)) StopMusicStream(bgm);
        if (delay < delayTarget) {
            delay++;
            return;
        }
        if (!IsMusicStreamPlaying(menu)) PlayMusicStream(menu);
        UpdateMusicStream(menu);
    // If the game is not started, play menu music
    } else {
        if (IsMusicStreamPlaying(bgm)) StopMusicStream(bgm);
        if (!IsMusicStreamPlaying(menu)) PlayMusicStream(menu);
        UpdateMusicStream(menu);
    }
}

void AudioManager::PlayShoot() {
    PlaySound(shootSound);
}

void AudioManager::PlayExplode() {
    PlaySound(explodeSound);
}

void AudioManager::PlayGameOver() {
    PlaySound(gameOverSound);
}
