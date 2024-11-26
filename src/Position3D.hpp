#ifndef _POSITION3D_HPP_
#define _POSITION3D_HPP_
#include "Position2D.hpp"

class Position3D : public Position2D {
private:
    float z;

public:
    Position3D() : Position2D(), z(0.0f) {}
    Position3D(float x, float y, float z) : Position2D(x, y), z(z) {}

    // Геттеры и сеттеры для z-координаты
    float getZ() const { return z; }
    void setZ(float newZ) { z = newZ; }

    // Переопределение методов для работы с 3D позицией
    void setPosition(float newX, float newY, float newZ) {
        Position2D::setPosition(newX, newY);
        z = newZ;
    }

    // Относительное перемещение
    void move(float dx, float dy, float dz) {
        Position2D::move(dx, dy);
        z += dz;
    }

    // Операторы сравнения
    bool operator==(const Position3D& other) const {
        return (Position2D::operator==(other)) && z == other.z;
    }

    bool operator!=(const Position3D& other) const {
        return !(*this == other);
    }

    // Получение расстояния до другой точки в 3D
    float distanceTo(const Position3D& other) const {
        float dx = getX() - other.getX();
        float dy = getY() - other.getY();
        float dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    // Нормализация позиции (приведение к единичному вектору)
    void normalize() {
        float length = std::sqrt(getX() * getX() + getY() * getY() + z * z);
        if (length != 0) {
            setX(getX() / length);
            setY(getY() / length);
            z /= length;
        }
    }
};

#endif // _POSITION3D_HPP_