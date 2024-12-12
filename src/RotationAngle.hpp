#ifndef _ROTATION_ANGLE_HPP_
#define _ROTATION_ANGLE_HPP_

#include <cmath>
#include "Position3D.hpp"

class RotationAngle {
private:
    float cosValue;
    float sinValue;

public:
    // Конструктор по умолчанию (угол 0 градусов)
    RotationAngle() : cosValue(1.0f), sinValue(0.0f) {}
    
    // Конструктор с заданными значениями
    RotationAngle(float cos, float sin) : cosValue(cos), sinValue(sin) {}
    
    // Конструктор из угла в радианах
    static RotationAngle fromRadians(float radians) {
        return RotationAngle(std::cos(radians), std::sin(radians));
    }
    
    // Геттеры
    float getCos() const { return cosValue; }
    float getSin() const { return sinValue; }
    
    // Сеттеры
    void setCos(float cos) { cosValue = cos; }
    void setSin(float sin) { sinValue = sin; }
    
    // Получить обратный угол (-angle)
    RotationAngle getInverse() const {
        return RotationAngle(cosValue, -sinValue);
    }
    
    // Получить угол в радианах
    float toRadians() const {
        return std::atan2(sinValue, cosValue);
    }

    int calculateRotationAngleYZ(Position3D coordinates) {
        float pY = coordinates.getY();
        float pZ = coordinates.getZ();
        
        if(pY != 0 || pZ != 0) {
            cosValue = (pY * 1.0f) / (sqrt(pow(pZ, 2) + pow(pY, 2)));
            sinValue =(pZ * 1.0f) / (sqrt(pow(pZ, 2) + pow(pY, 2)));
            return 0;
        } else {
            return 1;
        }
        
    }

    int calculateRotationAngleXY(Position3D coordinates) {
        float pX = coordinates.getX();
        float pY = coordinates.getY();
        
        if(pY != 0 || pX != 0) {
            cosValue = (pX * 1.0f) / (sqrt(pow(pX, 2) + pow(pY, 2)));
            sinValue = (pY * 1.0f) / (sqrt(pow(pX, 2) + pow(pY, 2)));
            return 0;
        } else {
            return 1;
        }
    }
};

#endif // _ROTATION_ANGLE_HPP_