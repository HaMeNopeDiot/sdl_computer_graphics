#ifndef _POSITION2D_HPP_
#define _POSITION2D_HPP_

#include <cmath>

class Position2D {
public:
    float x;
    float y;

    // Default constructor
    Position2D() : x(0.0f), y(0.0f) {}

    // Constructor with x, y
    Position2D(float x, float y) : x(x), y(y) {}


    // Copy constructor
    Position2D(const Position2D& other) = default;

    // Assignment operator
    Position2D& operator=(const Position2D& other) = default;

    // Utility methods
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }

    float getX() const { return x; }
    float getY() const { return y; }

    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    // Vector operations
    Position2D operator+(const Position2D& other) const {
        return Position2D(x + other.x, y + other.y);
    }

    Position2D operator-(const Position2D& other) const {
        return Position2D(x - other.x, y - other.y);
    }

    Position2D operator*(float scalar) const {
        return Position2D(x * scalar, y * scalar);
    }

    bool operator==(const Position2D& other) const {
        return x == other.x && y == other.y;
    }
};

#endif // _POSITION2D_HPP_
