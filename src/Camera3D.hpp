#pragma once
#include "Matrix.hpp"
#include <SDL2/SDL.h>
#include <cmath>

class Camera3D {
private:
    float N, F;        // Ближняя и дальняя плоскости отсечения
    int W, H;          // Размеры экрана
    Matrix viewMatrix;
    Matrix projectionMatrix;
    Matrix screenMatrix;

    void updateMatrices() {
        // Матрица вида (перемещает камеру в начало координат)
        viewMatrix = Matrix::identity(4);
        viewMatrix.at(2, 3) = -5.0f;  // Отодвигаем камеру назад
        
        // Матрица проекции (перспективная)
        projectionMatrix = Matrix(4, 4);
        float aspect = (float)W / H;
        float fov = 60.0f * M_PI / 180.0f;  // 60 градусов в радианы
        float tanHalfFov = tan(fov / 2.0f);
        
        projectionMatrix.at(0, 0) = 1.0f / (aspect * tanHalfFov);
        projectionMatrix.at(1, 1) = 1.0f / tanHalfFov;
        projectionMatrix.at(2, 2) = -(F + N) / (F - N);
        projectionMatrix.at(2, 3) = -(2.0f * F * N) / (F - N);
        projectionMatrix.at(3, 2) = -1.0f;
        
        // Матрица экрана (преобразует в экранные координаты)
        screenMatrix = Matrix(4, 4);
        screenMatrix.at(0, 0) = W / 2.0f;
        screenMatrix.at(1, 1) = -H / 2.0f;
        screenMatrix.at(0, 3) = W / 2.0f;
        screenMatrix.at(1, 3) = H / 2.0f;
        screenMatrix.at(2, 2) = 1.0f;
        screenMatrix.at(3, 3) = 1.0f;
    }

public:
    Camera3D(int width, int height) 
        : W(width), H(height), 
          N(0.1f), F(100.0f),
          viewMatrix(4, 4), 
          projectionMatrix(4, 4),
          screenMatrix(4, 4)
    {
        updateMatrices();
    }

    // Преобразование из мировых координат в экранные
    void worldToScreen(const Matrix& worldPoint, float& screenX, float& screenY) {
        // Применяем последовательно все преобразования
        Matrix viewPoint = viewMatrix * worldPoint;
        Matrix projPoint = projectionMatrix * viewPoint;
        
        // Перспективное деление
        float w = projPoint.at(3, 0);
        if (w != 0) {
            for (size_t i = 0; i < 4; ++i) {
                projPoint.at(i, 0) /= w;
            }
        }
        
        Matrix screenPoint = screenMatrix * projPoint;
        
        screenX = screenPoint.at(0, 0);
        screenY = screenPoint.at(1, 0);
    }

    // Отрисовка линии в мировых координатах
    void drawLine(SDL_Surface* surface, const Matrix& start, const Matrix& end, uint32_t color) {
        float x1, y1, x2, y2;
        worldToScreen(start, x1, y1);
        worldToScreen(end, x2, y2);
        
        // Используем алгоритм Брезенхэма для отрисовки линии
        int dx = abs((int)x2 - (int)x1);
        int dy = abs((int)y2 - (int)y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;

        int currentX = (int)x1;
        int currentY = (int)y1;

        while (true) {
            if (currentX >= 0 && currentX < W && currentY >= 0 && currentY < H) {
                ((uint32_t*)surface->pixels)[currentY * surface->pitch/4 + currentX] = color;
            }

            if (currentX == (int)x2 && currentY == (int)y2) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                currentX += sx;
            }
            if (e2 < dx) {
                err += dx;
                currentY += sy;
            }
        }
    }

    // Отрисовка координатных осей
    void drawAxes(SDL_Surface* surface) {
        float axisLength = 2.0f;  // Длина осей

        Matrix origin(4, 1);
        origin.at(3, 0) = 1.0f;

        // Положительные оси
        Matrix xAxis(4, 1);
        xAxis.at(0, 0) = axisLength;
        xAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, xAxis, 0xFF0000);  // Красная ось X

        Matrix yAxis(4, 1);
        yAxis.at(1, 0) = axisLength;
        yAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, yAxis, 0x00FF00);  // Зеленая ось Y

        Matrix zAxis(4, 1);
        zAxis.at(2, 0) = axisLength;
        zAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, zAxis, 0x0000FF);  // Синяя ось Z

        // Отрицательные оси (более тусклые)
        Matrix negXAxis(4, 1);
        negXAxis.at(0, 0) = -axisLength;
        negXAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negXAxis, 0x800000);  // Темно-красная ось -X

        Matrix negYAxis(4, 1);
        negYAxis.at(1, 0) = -axisLength;
        negYAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negYAxis, 0x008000);  // Темно-зеленая ось -Y

        Matrix negZAxis(4, 1);
        negZAxis.at(2, 0) = -axisLength;
        negZAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negZAxis, 0x000080);  // Темно-синяя ось -Z
    }

    void setScreenSize(int width, int height) {
        W = width;
        H = height;
        updateMatrices();
    }
};
