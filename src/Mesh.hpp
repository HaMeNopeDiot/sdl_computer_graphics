#ifndef _MESH_HPP_
#define _MESH_HPP_

#include <vector>
#include "Position2D.hpp"

class Mesh {
private:
    // Матрица вершин (3×M), где каждый столбец - это однородные координаты вершины
    std::vector<Position2D> vertices;
    
    // Матрица рёбер (K×2), где каждая строка содержит индексы двух связанных вершин
    struct Edge {
        int vertex1;
        int vertex2;
        
        Edge(int v1, int v2) : vertex1(v1), vertex2(v2) {}
    };
    std::vector<Edge> edges;

public:
    Mesh() = default;

    // Добавление вершины (возвращает индекс добавленной вершины)
    int addVertex(float x, float y) {
        vertices.emplace_back(x, y);
        return vertices.size() - 1;
    }

    // Добавление ребра между двумя вершинами
    void addEdge(int vertex1Index, int vertex2Index) {
        if (vertex1Index < vertices.size() && vertex2Index < vertices.size()) {
            edges.emplace_back(vertex1Index, vertex2Index);
        }
    }

    // Получение вершины по индексу
    const Position2D& getVertex(int index) const {
        return vertices[index];
    }

    // Получение ребра по индексу
    const Edge& getEdge(int index) const {
        return edges[index];
    }

    // Количество вершин
    size_t getVertexCount() const {
        return vertices.size();
    }

    // Количество рёбер
    size_t getEdgeCount() const {
        return edges.size();
    }

    // Очистка меша
    void clear() {
        vertices.clear();
        edges.clear();
    }

    // Получение всех вершин
    const std::vector<Position2D>& getVertices() const {
        return vertices;
    }

    // Получение всех рёбер
    const std::vector<Edge>& getEdges() const {
        return edges;
    }
};

#endif // _MESH_HPP_
