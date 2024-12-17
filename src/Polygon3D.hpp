#ifndef _POLYGON_3D_HPP_
#define _POLYGON_3D_HPP_

#include <vector>
#include "Position3D.hpp"

struct Polygon3D {
    std::vector<int> vertexIndices;  // Индексы вершин полигона
    Position3D normal;               // Нормаль полигона
    float avgZ;                      // Средняя Z-координата для сортировки

    Polygon3D(const std::vector<int>& indices) : vertexIndices(indices), normal(), avgZ(0.0f) {}

    // Вычисление нормали полигона
    void calculateNormal(const std::vector<Position3D>& vertices) {
        if (vertexIndices.size() < 3) return;
        
        Position3D v1 = vertices[vertexIndices[1]] - vertices[vertexIndices[0]];
        Position3D v2 = vertices[vertexIndices[2]] - vertices[vertexIndices[0]];
        normal = v1.cross(v2);
        normal.normalize();
    }

    // Вычисление средней Z-координаты полигона
    void calculateAverageZ(const std::vector<Position3D>& vertices) {
        if (vertexIndices.empty()) return;
        
        float sumZ = 0.0f;
        for (int index : vertexIndices) {
            sumZ += vertices[index].getZ();
        }
        avgZ = sumZ / vertexIndices.size();
    }
};

#endif // _POLYGON_3D_HPP_
