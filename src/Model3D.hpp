#ifndef _MODEL_3D_HPP_
#define _MODEL_3D_HPP_

#include "Matrix.hpp"
#include "Position3D.hpp"
#include "Camera3D.hpp"
#include <vector>
#include <memory>
#include <iostream>

class Model3D {
private:
    Matrix vertices;              // Исходные вершины модели
    Matrix transformedVertices;   // Текущие вершины после преобразований
    Matrix transformMatrix;       // Матрица накопленных преобразований
    std::vector<std::pair<int, int>> edges;  // Рёбра модели
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

    Position3D getVertexPos(int index) {
        return Position3D(vertices.at(0, index), vertices.at(1, index), vertices.at(2, index));
    }

    Position3D getTransformedVertexPos(int index) {
        return Position3D(transformedVertices.at(0, index), transformedVertices.at(1, index), transformedVertices.at(2, index));
    }

    // Обновление преобразованных вершин
    void updateTransformedVertices() {
        for (size_t i = 0; i < vertices.getCols(); ++i) {
            Matrix vertex(4, 1);
            for (size_t j = 0; j < 4; ++j) {
                vertex.at(j, 0) = vertices.at(j, i);
            }
            
            Matrix transformed = transformMatrix * vertex;
            for (size_t j = 0; j < 4; ++j) {
                transformedVertices.at(j, i) = transformed.at(j, 0);
            }
        }
    }

    // Отрисовка модели
    void draw(Camera3D& camera, SDL_Surface* surface, uint32_t color) {
        for (const auto& edge : edges) {
            Matrix v1(4, 1), v2(4, 1);
            
            // Получаем координаты первой вершины
            for (size_t i = 0; i < 4; ++i) {
                v1.at(i, 0) = transformedVertices.at(i, edge.first);
            }
            
            // Получаем координаты второй вершины
            for (size_t i = 0; i < 4; ++i) {
                v2.at(i, 0) = transformedVertices.at(i, edge.second);
            }
            
            // Отрисовываем ребро
            camera.drawLine(surface, v1, v2, color);
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
        return position;
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
};

#endif // _MODEL_3D_HPP_