#ifndef _MODEL_3D_HPP_
#define _MODEL_3D_HPP_

#include "Matrix.hpp"
#include "Position3D.hpp"
#include "Camera3D.hpp"
#include "HiddenSurfaceRemoval.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <set>


class Model3D {
private:
    Matrix vertices;              // Исходные вершины модели
    Matrix transformedVertices;   // Текущие вершины после преобразований
    Matrix transformMatrix;       // Матрица накопленных преобразований
    std::vector<std::pair<int, int>> edges;  // Рёбра модели

    std::vector<std::vector<int>> polygons;  // Полигоны модели
    HiddenSurfaceRemoval hsr;    // Обработчик удаления невидимых поверхностей
    Position3D position;         // Позиция модели в 3D пространстве

    float modelSizeXs;
    float modelSizeYs;
    float modelSizeZs;
    // SCALE
    float model_size_X;              // Размер модели по X
    float model_size_Y;              // Размер модели по Y
    float model_size_Z;              // Размер модели по Z

    // ROTAT
    float rotationX;
    float rotationY;
    float rotationZ;

public:
    Model3D(size_t vertexCount) 
        : vertices(4, vertexCount),
          transformedVertices(4, vertexCount),
          transformMatrix(Matrix::identity(4))
    {
        // Устанавливаем w-координату для всех вершин
        for (size_t i = 0; i < vertexCount; ++i) {
            vertices.at(3, i) = 1.0f;
        }
    }

    // Добавление вершины
    void setVertex(size_t index, float x, float y, float z) {
        if (index < vertices.getCols()) {
            vertices.at(0, index) = x;
            vertices.at(1, index) = y;
            vertices.at(2, index) = z;
            updateTransformedVertices();
        }
    }

    // Добавление ребра
    void addEdge(int v1, int v2) {
        if (v1 < vertices.getCols() && v2 < vertices.getCols()) {
            edges.push_back({v1, v2});
        }
    }

    // Применение аффинного преобразования
    void applyTransform(const Matrix& transform) {
        transformMatrix = transform * transformMatrix;
        updateTransformedVertices();
    }

    Position3D getVertexPos(int index) const {
        return Position3D(vertices.at(0, index), vertices.at(1, index), vertices.at(2, index));
    }

    Position3D getTransformedVertexPos(int index) const {
        return Position3D(transformedVertices.at(0, index), transformedVertices.at(1, index), transformedVertices.at(2, index));
    }

    // Обновление преобразованных вершин
    void updateTransformedVertices() {
        transformedVertices = transformMatrix * vertices;
        
        // Обновляем данные для удаления невидимых поверхностей
        std::vector<Position3D> transformedPositions;
        for (size_t i = 0; i < transformedVertices.getCols(); ++i) {
            transformedPositions.emplace_back(
                transformedVertices.at(0, i),
                transformedVertices.at(1, i),
                transformedVertices.at(2, i)
            );
        }
        hsr.setVertices(transformedPositions);
        hsr.updatePolygons();
    }

    // Отрисовка модели
    void draw(Camera3D& camera, SDL_Surface* surface, uint32_t color, uint32_t fill_color, Zbuffer &zbuffer) {
        // Получаем видимые полигоны
        auto visiblePolygons = getVisiblePolygons(camera);
        
        // Создаем множество видимых ребер
        std::set<std::pair<int, int>> visibleEdges;
        
        // Для каждого видимого полигона добавляем его ребра в множество видимых ребер
        for (const auto& polygon : visiblePolygons) {
            for (size_t i = 0; i < polygon.vertexIndices.size(); ++i) {
                int v1 = polygon.vertexIndices[i];
                int v2 = polygon.vertexIndices[(i + 1) % polygon.vertexIndices.size()];
                
                // Нормализуем ребро (меньший индекс всегда первый)
                if (v1 > v2) std::swap(v1, v2);
                visibleEdges.insert({v1, v2});
            }
        }
        
        size_t index = 0;
        for (const auto& polygon : visiblePolygons) {
            // Получаем координаты вершин полигона
            std::vector<Matrix> verticesQQQ;
            for (int index : polygon.vertexIndices) {
                Matrix vertex(4, 1);
                for (size_t i = 0; i < 4; ++i) {
                    vertex.at(i, 0) = transformedVertices.at(i, index);
                }
                verticesQQQ.push_back(vertex);
            }
            //std::cout << "=== VI: "  << index++ << " : " << polygon.vertexIndices[0] << " " << polygon.vertexIndices[1] << " " << polygon.vertexIndices[2] << std::endl;
            camera.fillTriangleAlt(surface, verticesQQQ[0], verticesQQQ[1], verticesQQQ[2], fill_color, zbuffer);
        }

        // Отрисовываем только видимые ребра
        for (const auto& edge : edges) {
            // Нормализуем ребро для сравнения
            int v1 = edge.first;
            int v2 = edge.second;
            if (v1 > v2) std::swap(v1, v2);
            
            // Проверяем, является ли ребро частью видимого полигона
            if (visibleEdges.find({v1, v2}) != visibleEdges.end()) {
                Matrix vert1(4, 1), vert2(4, 1);
                
                // Получаем координаты первой вершины
                for (size_t i = 0; i < 4; ++i) {
                    vert1.at(i, 0) = transformedVertices.at(i, edge.first);
                }
                
                // Получаем координаты второй вершины
                for (size_t i = 0; i < 4; ++i) {
                    vert2.at(i, 0) = transformedVertices.at(i, edge.second);
                }
                
                // Отрисовываем ребро с соответствующим цветом
                uint32_t edgeColor = color;
                if ((edge.first == 2 && edge.second == 3) || (edge.first == 3 && edge.second == 2)) {
                    edgeColor = 0xFFFF00;  // Желтый цвет
                }
                if ((edge.first == 3 && edge.second == 0) || (edge.first == 0 && edge.second == 3)) {
                    edgeColor = 0xFF00FF;  // Фиолетовый цвет
                }
                camera.drawLine(surface, vert1, vert2, edgeColor, zbuffer);
            }
        }
    }
    
    float getRotationX() const { return rotationX; }
    float getRotationY() const { return rotationY; }
    float getRotationZ() const { return rotationZ; }

    float get_model_size_X() const { return model_size_X; }
    float get_model_size_Y() const { return model_size_Y; }
    float get_model_size_Z() const { return model_size_Z; }

    float getModelSizeXs() const { return modelSizeXs; }
    float getModelSizeYs() const { return modelSizeYs; }
    float getModelSizeZs() const { return modelSizeZs; }

    // Получить текущую матрицу трансформации
    Matrix getTransformMatrix() const {
        return transformMatrix;
    }

    // Установить матрицу трансформации
    void setTransformMatrix(const Matrix& matrix) {
        transformMatrix = matrix;
        updateTransformedVertices();
    }

    // Добавление полигона
    void addPolygon(const std::vector<int>& vertexIndices) {
        switch(vertexIndices.size()) {
            case 3: {
                polygons.push_back(vertexIndices);
                hsr.addPolygon(vertexIndices);
                }
                break;
            case 4: {
                polygons.push_back(vertexIndices);
                hsr.addPolygon(vertexIndices);
                }
                break; 
            default: {
                std::cout << "not supported!" << std::endl;
                polygons.push_back(vertexIndices);
                hsr.addPolygon(vertexIndices);
                }
                break;
        }    
    }

    static std::shared_ptr<Model3D> createTriangle(float size = 1.0f)
    {
        auto triangle = std::make_shared<Model3D>(5);

        float halfSize = size / 2.0f;
        triangle->model_size_X = 1.0f;
        triangle->model_size_Y = 1.0f;
        triangle->model_size_Z = 1.0f;

        triangle->modelSizeXs = size;
        triangle->modelSizeYs = size;
        triangle->modelSizeZs = size;

        triangle->rotationX = 0.0f;
        triangle->rotationY = 0.0f;
        triangle->rotationZ = 0.0f;

        triangle->setVertex(0, -halfSize, -halfSize, -halfSize);  
        triangle->setVertex(1,  halfSize, -halfSize, -halfSize);  
        triangle->setVertex(2,  halfSize, -halfSize,  halfSize);
        triangle->setVertex(3, -halfSize, -halfSize,  halfSize);
        triangle->setVertex(4,  0,         halfSize,  0);

        triangle->addEdge(0, 1);
        triangle->addEdge(1, 2);
        triangle->addEdge(2, 3);
        triangle->addEdge(3, 0); 

        triangle->addEdge(0, 4);
        triangle->addEdge(1, 4);
        triangle->addEdge(2, 4);
        triangle->addEdge(3, 4); 


        
        // triangle->addPolygon({0, 1, 2, 3});
        triangle->addEdge(0, 2);
        triangle->addPolygon({0, 2, 3});
        triangle->addPolygon({2, 0, 1});
        
        triangle->addPolygon({1, 0, 4});
        triangle->addPolygon({2, 1, 4});
        triangle->addPolygon({3, 2, 4});
        triangle->addPolygon({0, 3, 4});

        return triangle;
    }

    // Создание куба
    static std::shared_ptr<Model3D> createCube(float size = 1.0f) {
        auto cube = std::make_shared<Model3D>(8);
        float halfSize = size / 2.0f;
        cube->model_size_X = 1.0f;
        cube->model_size_Y = 1.0f;
        cube->model_size_Z = 1.0f;

        cube->modelSizeXs = size;
        cube->modelSizeYs = size;
        cube->modelSizeZs = size;

        cube->rotationX = 0.0f;
        cube->rotationY = 0.0f;
        cube->rotationZ = 0.0f;

        // Вершины куба
        cube->setVertex(0, -halfSize, -halfSize, -halfSize);  // Задняя нижняя левая
        cube->setVertex(1, halfSize, -halfSize, -halfSize);   // Задняя нижняя правая
        cube->setVertex(2, halfSize, halfSize, -halfSize);    // Задняя верхняя правая
        cube->setVertex(3, -halfSize, halfSize, -halfSize);   // Задняя верхняя левая
        cube->setVertex(4, -halfSize, -halfSize, halfSize);   // Передняя нижняя левая
        cube->setVertex(5, halfSize, -halfSize, halfSize);    // Передняя нижняя правая
        cube->setVertex(6, halfSize, halfSize, halfSize);     // Передняя верхняя правая
        cube->setVertex(7, -halfSize, halfSize, halfSize);    // Передняя верхняя левая

        // Рёбра куба
        // Задняя грань
        cube->addEdge(0, 1);
        cube->addEdge(1, 2);
        cube->addEdge(2, 3);
        cube->addEdge(3, 0);
        // Передняя грань
        cube->addEdge(4, 5);
        cube->addEdge(5, 6);
        cube->addEdge(6, 7);
        cube->addEdge(7, 4);
        // Соединяющие рёбра
        cube->addEdge(0, 4);
        cube->addEdge(1, 5);
        cube->addEdge(2, 6);
        cube->addEdge(3, 7);

        // Добавляем полигоны (грани) куба
        // Передняя грань
        // cube->addPolygon({4, 5, 6, 7});
        // // Задняя грань
        // cube->addPolygon({0, 3, 2, 1});
        // // Верхняя грань
        // cube->addPolygon({3, 7, 6, 2});
        // // Нижняя грань
        // cube->addPolygon({0, 1, 5, 4});
        // // Левая грань
        // cube->addPolygon({0, 4, 7, 3});
        // // Правая грань
        // cube->addPolygon({1, 2, 6, 5});
        cube->addEdge(5, 7);
        cube->addEdge(0, 2);
        cube->addEdge(5, 2);
        cube->addEdge(1, 4);
        cube->addEdge(3, 4);
        cube->addEdge(7, 2);

        // Передняя грань
        cube->addPolygon({4, 5, 7});
        cube->addPolygon({5, 6, 7});

        // Задняя грань
        cube->addPolygon({0, 3, 2});
        cube->addPolygon({0, 2, 1});

        // Грань правая
        cube->addPolygon({5, 1, 2});
        cube->addPolygon({6, 5, 2});
        
        // Грань вверхняя
        cube->addPolygon({3, 7, 2});
        cube->addPolygon({7, 6, 2});

        // Грань левая
        cube->addPolygon({0, 4, 3});
        cube->addPolygon({4, 7, 3});

        // Грань нижняя
        cube->addPolygon({4, 0, 1});
        cube->addPolygon({4, 1, 5});
        return cube;
    }

    // Перемещение
    void translate(float dx, float dy, float dz) {
        position.move(dx, dy, dz);
        
        Matrix translation = Matrix::translation(position.getX(), position.getY(), position.getZ());
        transformMatrix = translation * Matrix::translation(-position.getX() + dx, -position.getY() + dy, -position.getZ() + dz) * transformMatrix;
        updateTransformedVertices();
    }

    void setPosition(float x, float y, float z) {
        position.setPosition(x, y, z);
        
        Matrix translation = Matrix::translation(x, y, z);
        transformMatrix = translation;
        updateTransformedVertices();
    }

    Position3D getPosition() const {
        Position3D firstModelEdge, secondModelEdge, centerModelEdge;
        firstModelEdge = getTransformedVertexPos(0);
        secondModelEdge = getTransformedVertexPos(0);

        for(size_t i = 0; i < vertices.getCols(); i++) {
            Position3D currentEdge = getTransformedVertexPos(i);
            if (currentEdge.getX() < firstModelEdge.getX()) {
                firstModelEdge.setX(currentEdge.getX());
            }
            if (currentEdge.getY() < firstModelEdge.getY()) {
                firstModelEdge.setY(currentEdge.getY());
            }
            if (currentEdge.getZ() < firstModelEdge.getZ()) {
                firstModelEdge.setZ(currentEdge.getZ());
            }
            if (currentEdge.getX() > secondModelEdge.getX()) {
                secondModelEdge.setX(currentEdge.getX());
            }
            if (currentEdge.getY() > secondModelEdge.getY()) {
                secondModelEdge.setY(currentEdge.getY());
            }
            if (currentEdge.getZ() > secondModelEdge.getZ()) {
                secondModelEdge.setZ(currentEdge.getZ());
            }
        }
        centerModelEdge.setX((firstModelEdge.getX() + secondModelEdge.getX()) * 1.0f / 2.0f);
        centerModelEdge.setY((firstModelEdge.getY() + secondModelEdge.getY()) * 1.0f / 2.0f);
        centerModelEdge.setZ((firstModelEdge.getZ() + secondModelEdge.getZ()) * 1.0f / 2.0f);
        // std::cout<< "centerModelEdge = " << centerModelEdge.getX() << " " << centerModelEdge.getY() << " " << centerModelEdge.getZ() << std::endl;
        return centerModelEdge;
        //return position;
    }

    // Масштабирование
    void scale(float sx, float sy, float sz) {
        applyTransform(Matrix::scaling(sx, sy, sz));
        this->model_size_X *= sx;
        this->model_size_Y *= sy;
        this->model_size_Z *= sz;
    }

    // Поворот вокруг оси X
    void rotateX(float angle) {
        applyTransform(Matrix::rotationX(angle));
        rotationX += angle * 1.0f;
    }

    // Поворот вокруг оси Y
    void rotateY(float angle) {
        applyTransform(Matrix::rotationY(angle));
        rotationY += angle * 1.0f;
    }

    // Поворот вокруг оси Z
    void rotateZ(float angle) {
        applyTransform(Matrix::rotationZ(angle));
        rotationZ += angle * 1.0f;
    }

    // Поворот вокруг оси X
    void rotateX(float cos, float sin) {
        applyTransform(Matrix::rotationX(cos, sin));
        // rotationX += angle * 1.0f;
    }

    // Поворот вокруг оси Y
    void rotateY(float cos, float sin) {
        applyTransform(Matrix::rotationY(cos, sin));
        // rotationY += angle * 1.0f;
    }

    // Поворот вокруг оси Z
    void rotateZ(float cos, float sin) {
        applyTransform(Matrix::rotationZ(cos, sin));
        // rotationZ += angle * 1.0f;
    }


    void reflectX() {
        applyTransform(Matrix::reflectZ());
    }
    
    void reflectY() {
        applyTransform(Matrix::reflectZ());
    }

    void reflectZ() {
        applyTransform(Matrix::reflectZ());
    }

    void RotateOverEdge(Position3D first, Position3D second, float deltaX) {
        applyTransform(Matrix::rotationOverEdge(first, second, deltaX));
    }

    void RotateOverEdge(size_t first, size_t second, float deltaX) {
        size_t countOfVertices = vertices.getCols();
        if(first < countOfVertices && second < countOfVertices && edgeExist(first, second)) {
            Position3D v1 = getTransformedVertexPos(first);
            Position3D v2 = getTransformedVertexPos(second);
            applyTransform(Matrix::rotationOverEdge(v1, v2, deltaX));
        }
        
    }

    bool edgeExist(int ver1, int ver2) {
        bool exist = false;
        for(size_t i = 0; i < edges.size(); i++) {
            std::pair<int, int> tmp  = {ver1, ver2};
            std::pair<int, int> tmpr = {ver2, ver1};
            if(tmp == edges[i] || tmpr == edges[i])
                exist = true;
        }
        return exist;
    }

    // Получение отсортированных видимых полигонов
    std::vector<Polygon3D> getVisiblePolygons(const Camera3D& camera) {
        auto sortedPolygons = hsr.sortPolygons();
        std::vector<Polygon3D> visiblePolygons;
        
        // size_t index = 0;
        // std::cout<< "List" << std::endl;
        for (const auto& polygon : sortedPolygons) {
            if (hsr.isPolygonVisible(polygon, camera.getPosition())) {
                visiblePolygons.push_back(polygon);
                // std::cout << index << " of " << polygons.size() << " visible." << std::endl;
            }
            // index++;
        }
        
        return visiblePolygons;
    }

    std::vector<std::vector<int>> getPolygons()
    {
        return polygons;  // Полигоны модели
    }

    Matrix getTransformedVertices()
    {
        return transformedVertices;
    }
};

#endif // _MODEL_3D_HPP_