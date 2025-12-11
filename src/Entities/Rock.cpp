#include "Rock.h"

using PhysConst = Constants::Physics;

Rock::Rock(Vector2 pos, float radius) : position(pos), active(true), radius(radius) {
}

void Rock::Update() {
    if (!active) return;

    position.x += PhysConst::RockSpeed;
}

void Rock::Draw() const {
    if (active) {
        // Draw a few ellipses to simulate a rock
        DrawCircleV(position, radius, BROWN);
        DrawCircleV(position, radius * 0.8f, BROWN);
        DrawCircleV(position, radius * 0.6f, BROWN);
    }
}

Rectangle Rock::GetRect() const {
    return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
}