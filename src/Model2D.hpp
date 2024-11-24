#ifndef _MODEL_2D_HPP_
#define _MODEL_2D_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <algorithm>

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

    // Shape management methods
    void addShape(std::shared_ptr<Shape2D> shape) {
        if (shape) {
            shapes.push_back(shape);
        }
    }

    void removeShape(const std::shared_ptr<Shape2D>& shape) {
        auto it = std::find(shapes.begin(), shapes.end(), shape);
        if (it != shapes.end()) {
            shapes.erase(it);
        }
    }

    void removeShapeAt(size_t index) {
        if (index < shapes.size()) {
            shapes.erase(shapes.begin() + index);
        }
    }

    void clearShapes() {
        shapes.clear();
    }

    // Add shapes
    void addLine(float x1, float y1, float x2, float y2, Uint32 color, float thickness = 1.0f);
    void addCurve(float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color);

    // Getters for shapes
    size_t getShapeCount() const {
        return shapes.size();
    }

    std::shared_ptr<Shape2D> getShape(size_t index) const {
        if (index < shapes.size()) {
            return shapes[index];
        }
        return nullptr;
    }

    const std::vector<std::shared_ptr<Shape2D>>& getShapes() const {
        return shapes;
    }

    // Drawing method
    void draw(SDL_Surface* surface, Camera2D& camera);

    // Getters
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getRotation() const { return rotation; }
    float getScaleX() const { return scaleX; }
    float getScaleY() const { return scaleY; }
};

#endif // _MODEL_2D_HPP_