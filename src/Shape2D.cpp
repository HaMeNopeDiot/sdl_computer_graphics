#include "Shape2D.hpp"

// Helper function to rotate a point around origin
void rotatePoint(float& x, float& y, float degrees) {
    float radians = degrees * M_PI / 180.0f;
    float cosA = cos(radians);
    float sinA = sin(radians);
    float newX = x * cosA - y * sinA;
    float newY = x * sinA + y * cosA;
    x = newX;
    y = newY;
}

void Line2D::draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) {
    // Apply transformations
    float transformedX1 = x1 * scaleX;
    float transformedY1 = y1 * scaleY;
    float transformedX2 = x2 * scaleX;
    float transformedY2 = y2 * scaleY;

    // Rotate points
    rotatePoint(transformedX1, transformedY1, rotation);
    rotatePoint(transformedX2, transformedY2, rotation);

    // Translate points
    transformedX1 += modelX;
    transformedY1 += modelY;
    transformedX2 += modelX;
    transformedY2 += modelY;

    // Draw the transformed line
    drawLine(surface, camera, transformedX1, transformedY1, transformedX2, transformedY2, color, thickness);
}

void Curve2D::draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) {
    // Apply transformations
    float transformedX1 = x1 * scaleX;
    float transformedY1 = y1 * scaleY;
    float transformedCX = cx * scaleX;
    float transformedCY = cy * scaleY;
    float transformedX2 = x2 * scaleX;
    float transformedY2 = y2 * scaleY;

    // Rotate points
    rotatePoint(transformedX1, transformedY1, rotation);
    rotatePoint(transformedCX, transformedCY, rotation);
    rotatePoint(transformedX2, transformedY2, rotation);

    // Translate points
    transformedX1 += modelX;
    transformedY1 += modelY;
    transformedCX += modelX;
    transformedCY += modelY;
    transformedX2 += modelX;
    transformedY2 += modelY;

    // Draw the transformed curve
    drawQuadraticCurve(surface, camera, transformedX1, transformedY1, transformedCX, transformedCY, 
                       transformedX2, transformedY2, color);
}
