#pragma once
#include "raylib.h"

enum ShapeType {
    RECTANGLE,
    ELLIPSE,
    TRIANGLE,
    RING
};

enum ShapeID {
    NONE,
    MAIN_ROTOR,
    TAIL_ROTOR
};

struct Shape {
    ShapeType type;
    Rectangle rect;
    Color color;
    ShapeID id = NONE;
    float rotation = 0.0f;
    float param = 0.0f; // For RING: thickness

    void Draw(Vector2 offset) const;
};
