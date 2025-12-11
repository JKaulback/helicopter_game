#include "Shape.h"
#include "rlgl.h"

void Shape::Draw(Vector2 offset) const {
    float posX = offset.x + rect.x;
    float posY = offset.y + rect.y;

    if (rotation != 0.0f) {
        float centerX = posX + rect.width / 2.0f;
        float centerY = posY + rect.height / 2.0f;
        
        rlPushMatrix();
        rlTranslatef(centerX, centerY, 0);
        rlRotatef(rotation, 0, 0, 1);
        rlTranslatef(-centerX, -centerY, 0);
    }

    switch (type) {
        case RECTANGLE:
            DrawRectangle((int)posX, (int)posY, (int)rect.width, (int)rect.height, color);
            break;
        case ELLIPSE:
            DrawEllipse((int)posX, (int)posY, (int)rect.width, (int)rect.height, color);
            break;
        case TRIANGLE:
            // Draw an isosceles triangle fitting in the rect
            // Point 1: Top Center
            // Point 2: Bottom Right
            // Point 3: Bottom Left
            DrawTriangle(
                {posX + rect.width / 2, posY}, 
                {posX, posY + rect.height}, 
                {posX + rect.width, posY + rect.height}, 
                color
            );
            break;
        case RING:
            float radius = rect.width / 2.0f;
            DrawRing(
                {posX + radius, posY + radius}, 
                radius - param, // Inner radius
                radius,         // Outer radius
                0, 360, 0,      // Start/End angle, segments
                color
            );
            break;
    }

    if (rotation != 0.0f) {
        rlPopMatrix();
    }
}
