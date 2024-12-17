#ifndef _HIDDEN_SURFACE_REMOVAL_HPP_
#define _HIDDEN_SURFACE_REMOVAL_HPP_

#include <vector>
#include <algorithm>
#include "Polygon3D.hpp"
#include "Position3D.hpp"

class HiddenSurfaceRemoval {
private:
    std::vector<Polygon3D> polygons;
    std::vector<Position3D> vertices;

public:
    // Добавление полигона
    void addPolygon(const std::vector<int>& vertexIndices) {
        polygons.emplace_back(vertexIndices);
    }

    // Установка вершин
    void setVertices(const std::vector<Position3D>& newVertices) {
        vertices = newVertices;
    }

    // Обновление данных полигонов
    void updatePolygons() {
        for (auto& polygon : polygons) {
            polygon.calculateNormal(vertices);
            polygon.calculateAverageZ(vertices);
        }
    }

    // Сортировка полигонов по Z (алгоритм художника)
    std::vector<Polygon3D> sortPolygons() {
        std::vector<Polygon3D> sortedPolygons = polygons;
        
        // Сортировка от дальних к ближним (по убыванию Z)
        std::sort(sortedPolygons.begin(), sortedPolygons.end(),
            [](const Polygon3D& a, const Polygon3D& b) {
                return a.avgZ > b.avgZ;
            });
        
        return sortedPolygons;
    }

    // Проверка видимости полигона
    bool isPolygonVisible(const Polygon3D& polygon, const Position3D& cameraPos) {
        // Вектор от любой точки полигона к камере
        Position3D toCamera = cameraPos - vertices[polygon.vertexIndices[0]];
        
        // Скалярное произведение нормали и вектора к камере
        float dotProduct = polygon.normal.dot(toCamera);
        
        // Если скалярное произведение положительное, полигон видим
        return dotProduct > 0;
    }
};

#endif // _HIDDEN_SURFACE_REMOVAL_HPP_
