
#ifndef _MODEL_2D_HPP_
#define _MODEL_2D_HPP_


#include <SDL2/SDL.h>
#include <vector>
#include <memory>

#include "Shape2D.hpp"
#include "Camera2D.hpp"

// Forward declaration
class Shape2D;

class Model2D {
private:
    std::vector<std::shared_ptr<Shape2D>> shapes;
    float posX, posY;    // Position in normalized coordinates
    float rotation;      // Rotation in degrees
    float scaleX, scaleY;// Scale factors

public:
    Model2D() : posX(0.0f), posY(0.0f), rotation(0.0f), scaleX(1.0f), scaleY(1.0f) {}

    // Transform methods
    void setPosition(float x, float y) {
        posX = x;
        posY = y;
    }

    void move(float dx, float dy) {
        posX += dx;
        posY += dy;
    }

    void setRotation(float degrees) {
        rotation = degrees;
    }

    void rotate(float degrees) {
        rotation += degrees;
    }

    void setScale(float sx, float sy) {
        scaleX = sx;
        scaleY = sy;
    }

    // Add shapes
    void addLine(float x1, float y1, float x2, float y2, Uint32 color, float thickness = 1.0f);
    void addCurve(float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color);

    // Draw the model
    void draw(SDL_Surface* surface, Camera2D& camera);

    // Getters
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getRotation() const { return rotation; }
    float getScaleX() const { return scaleX; }
    float getScaleY() const { return scaleY; }
};

#endif // _MODEL_2D_HPP_