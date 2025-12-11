#include "Explosion.h"

Explosion::Explosion(Vector2 pos) 
    : position(pos), timer(0.5f), active(true) {
}

void Explosion::Update() {
    if (!active) return;

    timer -= GetFrameTime();
    if (timer <= 0) {
        active = false;
    }
}

void Explosion::Draw() const {
    if (!active) return;

    float radius = (0.5f - timer) * 80.0f; // Expand
    Color col = (timer > 0.25f) ? ORANGE : YELLOW; // Fade color
    col.a = (unsigned char)(timer * 2.0f * 255.0f); // Fade alpha
    
    DrawCircle((int)position.x, (int)position.y, radius, col);
}
