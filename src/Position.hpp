#ifndef _POSITION_HPP_
#define _POSITION_HPP_


class Position {
public:
    float x;
    float y;
    float z;

    // Default constructor
    Position() : x(0.0f), y(0.0f), z(0.0f) {}

    // Constructor with x, y (z defaults to 0)
    Position(float x, float y) : x(x), y(y), z(0.0f) {}

    // Constructor with x, y, z
    Position(float x, float y, float z) : x(x), y(y), z(z) {}

    // Copy constructor
    Position(const Position& other) = default;

    // Assignment operator
    Position& operator=(const Position& other) = default;

    // Utility methods
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void setPosition(float newX, float newY, float newZ) {
        x = newX;
        y = newY;
        z = newZ;
    }

    // Vector operations
    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y, z + other.z);
    }

    Position operator-(const Position& other) const {
        return Position(x - other.x, y - other.y, z - other.z);
    }

    Position operator*(float scalar) const {
        return Position(x * scalar, y * scalar, z * scalar);
    }
};

#endif // _POSITION_HPP_
