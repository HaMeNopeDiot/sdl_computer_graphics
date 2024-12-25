# ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <vector>
#include <cmath>

#include "Position3D.hpp"
#include "RotationAngle.hpp"

class Matrix {
private:
    std::vector<std::vector<float>> data;
    size_t rows, cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<float>(cols, 0.0f));
    }

    // Создание единичной матрицы
    static Matrix identity(size_t size) {
        Matrix m(size, size);
        for (size_t i = 0; i < size; ++i) {
            m.data[i][i] = 1.0f;
        }
        return m;
    }

    float& at(size_t row, size_t col) {
        return data[row][col];
    }

    const float& at(size_t row, size_t col) const {
        return data[row][col];
    }

    // Умножение матриц
    Matrix operator*(const Matrix& other) const {
        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < cols; ++k) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
        return result;
    }

    // Создание матрицы переноса
    static Matrix translation(float dx, float dy, float dz) {
        Matrix m = identity(4);
        m.at(0, 3) = dx;
        m.at(1, 3) = dy;
        m.at(2, 3) = dz;
        return m;
    }

    static Matrix translation(Position3D *point) {
        Matrix m = identity(4);
        m.at(0, 3) = point->getX();
        m.at(1, 3) = point->getY();
        m.at(2, 3) = point->getZ();
        return m;
    }

    // Создание матрицы масштабирования
    static Matrix scaling(float sx, float sy, float sz) {
        Matrix m = identity(4);
        m.at(0, 0) = sx;
        m.at(1, 1) = sy;
        m.at(2, 2) = sz;
        return m;
    }

    // Создание матрицы поворота вокруг оси X
    static Matrix rotationX(float angle) {
        Matrix m = identity(4);
        float c = cos(angle);
        float s = sin(angle);
        m.at(1, 1) = c;         /* | 1    0    0   0 | */
        m.at(1, 2) = -s;        /* | 0    c   -s   0 | */
        m.at(2, 1) = s;         /* | 0    s    c   0 | */
        m.at(2, 2) = c;         /* | 0    0    0   1 | */
        return m;
    }

    // Создание матрицы поворота вокруг оси Y
    static Matrix rotationY(float angle) {
        Matrix m = identity(4);
        float c = cos(angle);
        float s = sin(angle);
        m.at(0, 0) = c;         /* | c    0    s   0 | */
        m.at(0, 2) = s;         /* | 0    1    0   0 | */
        m.at(2, 0) = -s;        /* |-s    0    c   0 | */
        m.at(2, 2) = c;         /* | 0    0    0   1 | */
        return m;
    }

    // Создание матрицы поворота вокруг оси Z
    static Matrix rotationZ(float angle) {
        Matrix m = identity(4);
        float c = cos(angle);
        float s = sin(angle);
        m.at(0, 0) = c;
        m.at(0, 1) = -s;
        m.at(1, 0) = s;
        m.at(1, 1) = c;
        return m;
    }

    static Matrix rotationZ(float cos, float sin) {
        Matrix m = identity(4);
        m.at(0, 0) = cos;
        m.at(0, 1) = -sin;
        m.at(1, 0) = sin;
        m.at(1, 1) = cos;
        return m;
    }

        static Matrix rotationY(float cos, float sin) {
        Matrix m = identity(4);
        m.at(0, 0) = cos;         /* | c    0    s   0 | */
        m.at(0, 2) = sin;         /* | 0    1    0   0 | */
        m.at(2, 0) = -sin;        /* |-s    0    c   0 | */
        m.at(2, 2) = cos;         /* | 0    0    0   1 | */
        return m;
    }

    static Matrix rotationX(float cos, float sin) {
        Matrix m = identity(4);
        m.at(1, 1) = cos;         /* | 1    0    0   0 | */
        m.at(1, 2) = -sin;        /* | 0    c   -s   0 | */
        m.at(2, 1) = sin;         /* | 0    s    c   0 | */
        m.at(2, 2) = cos;         /* | 0    0    0   1 | */
        return m;
    }
    

    static Matrix reflectX() {
        Matrix m = identity(4);
        m.at(0, 0) = -1;
        return m;
    }

    static Matrix reflectY() {
        Matrix m = identity(4);
        m.at(1, 1) = -1;
        return m;
    }

    static Matrix reflectZ() {
        Matrix m = identity(4);
        m.at(2, 2) = -1;
        return m;
    }

    static Matrix rotationOverEdge(Position3D first, Position3D second, float deltaX)
    {
        Matrix m = identity(4);

        // Первая точка ребра будет помещена в начало координато
        Position3D firstReverse(-first.getX(), -first.getY(), -first.getZ());

        Matrix toCenter = translation(firstReverse.getX(), firstReverse.getY(), firstReverse.getZ());
        m = toCenter * m;

        second.setPosition(second.getX() - first.getX(),
                           second.getY() - first.getY(),
                           second.getZ() - first.getZ());
        // Первый поворот
        RotationAngle angleAlpha;
        int failure_x = angleAlpha.calculateRotationAngleYZ(second);
        
        float cosAlpha = angleAlpha.getCos();
        float sinAlpha = angleAlpha.getSin();

        if(!failure_x) {
            // Поворот по ОХ
            m = rotationX(cosAlpha, -sinAlpha) * m;
            // Высчитываем новые координаты вершины
            float newPointY = sqrt(pow(second.getY(), 2) + pow(second.getZ(), 2));
            second.setPosition(second.getX(), newPointY, 0.0f);
        }

        // Второй поворот
        RotationAngle angleBetta;
        int failure_z = angleBetta.calculateRotationAngleXY(second);
        
        float cosBetta = angleBetta.getCos();
        float sinBetta = angleBetta.getSin();

        if(!failure_z) {
            m = rotationZ(cosBetta, -sinBetta) * m;
        }
        
        // Поворот по ребру
        m = rotationX(deltaX * 0.0025f) * m;

        // Обратный повороты
        if(!failure_z) { m = rotationZ(cosBetta, sinBetta)   * m; }
        if(!failure_x) { m = rotationX(cosAlpha, sinAlpha) * m; }
        
        Matrix toPrev = translation(first.getX(), first.getY(), first.getZ());
        m = toPrev * m;
        return m;
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
};

#endif /* _MATRIX_HPP_ */