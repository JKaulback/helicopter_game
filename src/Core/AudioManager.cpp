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
}

void AudioManager::Shutdown() {
    UnloadSound(shootSound);
    UnloadSound(explodeSound);
    UnloadSound(gameOverSound);
    
    UnloadMusicStream(bgm);
    UnloadMusicStream(menu);
    
    CloseAudioDevice();
}

void AudioManager::UpdateMusic(bool isLevelActive) {
    if (isLevelActive) {
        if (IsMusicStreamPlaying(menu)) StopMusicStream(menu);
        if (!IsMusicStreamPlaying(bgm)) PlayMusicStream(bgm);
        UpdateMusicStream(bgm);
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
