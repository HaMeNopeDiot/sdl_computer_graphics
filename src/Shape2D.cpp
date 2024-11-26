#include "Shape2D.hpp"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Helper function to rotate a Position2D
void rotatePosition(Position2D& pos, float degrees) {
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
    Position2D transformedStart(start.x * scaleX, start.y * scaleY);
    Position2D transformedEnd(end.x * scaleX, end.y * scaleY);

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
    Position2D transformedStart(start.x * scaleX, start.y * scaleY);
    Position2D transformedControl(control.x * scaleX, control.y * scaleY);
    Position2D transformedEnd(end.x * scaleX, end.y * scaleY);

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
                       transformedEnd.x, transformedEnd.y,
                       color);
}
