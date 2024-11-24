#include "Shape2D.hpp"

// Helper function to rotate a Position
void rotatePosition(Position& pos, float degrees) {
    float radians = degrees * M_PI / 180.0f;
    float cosA = cos(radians);
    float sinA = sin(radians);
    float newX = pos.x * cosA - pos.y * sinA;
    float newY = pos.x * sinA + pos.y * cosA;
    pos.x = newX;
    pos.y = newY;
}

void Line2D::draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) {
    // Create transformed positions
    Position transformedStart(start.x * scaleX, start.y * scaleY);
    Position transformedEnd(end.x * scaleX, end.y * scaleY);

    // Rotate points
    rotatePosition(transformedStart, rotation);
    rotatePosition(transformedEnd, rotation);

    // Translate points
    transformedStart.x += modelX;
    transformedStart.y += modelY;
    transformedEnd.x += modelX;
    transformedEnd.y += modelY;

    // Draw the transformed line
    drawLine(surface, camera, transformedStart.x, transformedStart.y, 
             transformedEnd.x, transformedEnd.y, color, thickness);
}

void Curve2D::draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) {
    // Create transformed positions
    Position transformedStart(start.x * scaleX, start.y * scaleY);
    Position transformedControl(control.x * scaleX, control.y * scaleY);
    Position transformedEnd(end.x * scaleX, end.y * scaleY);

    // Rotate points
    rotatePosition(transformedStart, rotation);
    rotatePosition(transformedControl, rotation);
    rotatePosition(transformedEnd, rotation);

    // Translate points
    transformedStart.x += modelX;
    transformedStart.y += modelY;
    transformedControl.x += modelX;
    transformedControl.y += modelY;
    transformedEnd.x += modelX;
    transformedEnd.y += modelY;

    // Draw the transformed curve
    drawQuadraticCurve(surface, camera, 
                       transformedStart.x, transformedStart.y,
                       transformedControl.x, transformedControl.y,
                       transformedEnd.x, transformedEnd.y, color);
}
