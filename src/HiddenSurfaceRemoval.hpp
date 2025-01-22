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

        // std::cout << "debug b: " << sortedPolygons.size() << " : ";
        // for(size_t i = 0; i < sortedPolygons.size(); i++) {
        //     Polygon3D tmp = sortedPolygons[i];
        //     std::cout << " " << tmp.getAvgZ();
        // }
        // std::cout << std::endl;

        // Сортировка от дальних к ближним (по убыванию Z)
        std::sort(sortedPolygons.begin(), sortedPolygons.end(),
            [](const Polygon3D& a, const Polygon3D& b) {
                return a.avgZ < b.avgZ;
            });

        // std::cout << "debug b: " << sortedPolygons.size() << " : ";
        // for(size_t i = 0; i < sortedPolygons.size(); i++) {
        //     Polygon3D tmp = sortedPolygons[i];
        //     std::cout << " " << tmp.getAvgZ();
        // }
        // std::cout << std::endl;
        
        return sortedPolygons;
    }

    // Проверка видимости полигона
    bool isPolygonVisible(Polygon3D polygon, Position3D cameraPos) {
        // Вектор от любой точки полигона к камере
        Position3D toCamera = cameraPos - vertices[polygon.vertexIndices[0]];
        // cameraPos.printPos();
        
        // std::cout << "cameraPos: ";
        // cameraPos.printPos();
        // std::cout << "toCamera: ";
        // toCamera.printPos();
        // Скалярное произведение нормали и вектора к камере
        Position3D normal = polygon.normal;
        std::vector<int> vectorVerticies = polygon.getVertexIndices();
        // for(size_t i = 0; i < vectorVerticies.size(); i++) {
        //     // std::cout << "i:" << vectorVerticies[i] << "-pol:" << vertices[polygon.vertexIndices[i]].dot(cameraPos - vertices[polygon.vertexIndices[i]]) << std::endl;
        //     std::cout << "i:" << vectorVerticies[i] << "-pol:" <<  normal.dot(vertices[polygon.vertexIndices[i]]) << " camera: " << vertices[polygon.vertexIndices[i]].dot(cameraPos - vertices[polygon.vertexIndices[i]]) << std::endl;
        // }

        float dotProduct = polygon.normal.dot(toCamera);
        // std::cout << "- : - " << dotProduct << " POL:" << polygon.getAvgZ() << std::endl;


        // negneg
        Position3D edge1 = vertices[polygon.vertexIndices[1]] - vertices[polygon.vertexIndices[0]];
        Position3D edge2 = vertices[polygon.vertexIndices[2]] - vertices[polygon.vertexIndices[0]];

        Position3D normal_e = edge1.cross(edge2);
        normal_e.normalize();
        Position3D toCamera2 = cameraPos - normal_e;
        toCamera.normalize();
        // std::cout<< "MEOW: " << normal_e.dot(toCamera2) << std::endl;
        
        // Если скалярное произведение положительное, полигон видим
        return ((dotProduct > 0) && (polygon.size() % 2 == 0)) || ((dotProduct > 0) && (polygon.size() % 2 != 0));
    }

    std::vector<Polygon3D> getPolygons()
    {
        return polygons;
    }
};

#endif // _HIDDEN_SURFACE_REMOVAL_HPP_
