# ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <vector>
#include <cmath>

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

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
};

#endif /* _MATRIX_HPP_ */