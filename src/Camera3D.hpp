#ifndef _CAMERA_3D_HPP_
#define _CAMERA_3D_HPP_

#include "Matrix.hpp"
#include "Position3D.hpp"
#include "Position2D.hpp"
#include "Zbuffer.hpp"

#include <SDL2/SDL.h>
#include <limits>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <cmath>
#include <algorithm>

class Camera3D {
private:
    int W, H;          // Размеры экрана
    float N, F;        // Ближняя и дальняя плоскости отсечения
    // Матрица вида - преобразует координаты из мирового пространства в пространство камеры
    Matrix viewMatrix;
    // Матрица проекции - преобразует координаты из пространства камеры в нормализованные координаты устройства
    Matrix projectionMatrix;
    // Матрица экрана - преобразует нормализованные координаты в экранные координаты
    Matrix screenMatrix;
    Position3D position;  // Позиция камеры
    float yaw = 0.0f;    // Поворот вокруг оси Y
    float pitch = 0.0f;  // Поворот вокруг оси X


    std::vector<std::vector<int>> zBuffer;

    void initZBuffer() {
        std::vector<int> zBufferRow (W, std::numeric_limits<int>::max());
    }

    void updateMatrices() {
        // Матрица вида (View Matrix)
        // Преобразует координаты из мирового пространства в пространство камеры
        // Это достигается путем перемещения камеры в начало координат и поворота сцены в противоположном направлении
        viewMatrix = Matrix::identity(4);
        
        // Создаем матрицу поворота, комбинируя повороты вокруг осей:
        // pitch - поворот вокруг оси X (наклон камеры вверх-вниз)
        // yaw - поворот вокруг оси Y (поворот камеры влево-вправо)
        Matrix rotation = Matrix::rotationX(pitch) * Matrix::rotationY(yaw);
        
        // Создаем матрицу перемещения
        // Используем отрицательные координаты, так как перемещаем сцену в противоположном направлении
        Matrix translation = Matrix::translation(-position.getX(), -position.getY(), -position.getZ());
        
        // Итоговая матрица вида = поворот * перемещение
        viewMatrix = rotation * translation;
        
        // Матрица перспективной проекции (Perspective Projection Matrix)
        // Преобразует координаты из пространства камеры в нормализованные координаты устройства
        projectionMatrix = Matrix(4, 4);
        float aspect = (float)W / H;  // Соотношение сторон экрана
        float fov = 60.0f * M_PI / 180.0f;  // Угол обзора (Field of View) в радианах
        float tanHalfFov = tan(fov / 2.0f);  // Тангенс половинного угла обзора
        
        // Заполняем матрицу проекции:
        projectionMatrix.at(0, 0) = 1.0f / (aspect * tanHalfFov);  // Масштаб по X с учетом aspect ratio           |                                                                                                     |   
        projectionMatrix.at(1, 1) = 1.0f / tanHalfFov;             // Масштаб по Y                                 | 1.0f / (aspect * tanHalfFov)            0                    0                            0         |
        projectionMatrix.at(2, 2) = -(F + N) / (F - N);            // Масштаб по Z                                 |                0                 1.0f/tanHalfFov             0                            0         |
        projectionMatrix.at(2, 3) = -(2.0f * F * N) / (F - N);     // Перенос по Z                                 |                0                        0           -(F + N) / (F - N)    -(2.0f * F * N) / (F - N) |
        projectionMatrix.at(3, 2) = -1.0f;                         // W-координата для перспективного деления      |                0                        0                    1                            0         |     
        
        // Матрица экрана (Screen Matrix)
        // Преобразует нормализованные координаты устройства в экранные координаты
        screenMatrix = Matrix(4, 4);
        screenMatrix.at(0, 0) = W / 2.0f;    // Масштаб по X                                                |                                  |
        screenMatrix.at(1, 1) = -H / 2.0f;   // Масштаб по Y (отрицательный, так как Y растет вниз)         | W / 2.0f     0      0   W / 2.0f | 
        screenMatrix.at(0, 3) = W / 2.0f;    // Смещение по X в центр экрана                                |    0     -H / 2.0f  0   H / 2.0f | 
        screenMatrix.at(1, 3) = H / 2.0f;    // Смещение по Y в центр экрана                                |    0         0      1      0     | 
        screenMatrix.at(2, 2) = 1.0f;        // Сохраняем Z-координату                                      |    0         0      0      1     |   
        screenMatrix.at(3, 3) = 1.0f;        // W-компонента                                                |                                  |  
    }

public:
    Camera3D(int width, int height) 
        : W(width), H(height), 
          N(0.1f), F(100.0f),
          viewMatrix(4, 4), 
          projectionMatrix(4, 4),
          screenMatrix(4, 4),
          position(0.0f, 0.0f, 5.0f)  // Начальная позиция камеры
    {
        updateMatrices();
    }

    // Преобразование из мировых координат в экранные
    void worldToScreen(const Matrix& worldPoint, float& screenX, float& screenY, float& screenW) {
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
        screenW = w;
        
        // screenPoint.printMatrix();
    }

    // Отрисовка линии в мировых координатах
    void drawLine(SDL_Surface* surface, const Matrix& start, const Matrix& end, uint32_t color, Zbuffer& zbuffer) {
        float x1, y1, x2, y2, w1, w2;
        worldToScreen(start, x1, y1, w1);
        worldToScreen(end, x2, y2, w2);

        w1 = 1.0f / w1;
        w2 = 1.0f / w2;
        
        // Используем алгоритм Брезенхэма для отрисовки линии
        int   dx = abs((int)x2 - (int)x1);
        int   dy = abs((int)y2 - (int)y1);
        float dw = abs((float)w2 - (float)w1);

        int   sx = x1 < x2 ? 1 : -1;
        int   sy = y1 < y2 ? 1 : -1;
        float sw = ((float)w2 - (float)w1) * 1.0f / abs((float)x2 - (float)x1);

        int err = dx - dy;

        int currentX   = (int)x1;
        int currentY   = (int)y1;
        float currentW = (float)w1;

        int stepsX = 0;
        while (true) {
            float z = 1.0f / currentW;
            if (currentX >= 0 && currentX < W && currentY >= 0 && currentY < H) {
                if(z < zbuffer.getValue(currentX, currentY)) {
                    zbuffer.setValue(currentX, currentY, z);
                    ((uint32_t*)surface->pixels)[currentY * surface->pitch/4 + currentX] = color;   
                }
            }

            if (currentX == (int)x2 && currentY == (int)y2) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                currentX += sx;

                stepsX++;
                currentW = w1 + sw * (float)stepsX;
                
            }
            if (e2 < dx) {
                err += dx;
                currentY += sy;
            }

            
        }
    }

    void fillTriangle(SDL_Surface* surface, const Matrix& v1, const Matrix& v2, const Matrix& v3, uint32_t color) {
        float x1, y1, x2, y2, x3, y3, w1, w2, w3;
        worldToScreen(v1, x1, y1, w1);
        worldToScreen(v2, x2, y2, w2);
        worldToScreen(v3, x3, y3, w3);
        
        // std::cout << "START" << std::endl;
        // std::cout << "xy1:" << x1 << " " << y1 << std::endl;
        // std::cout << "xy2:" << x2 << " " << y2 << std::endl;
        // std::cout << "xy3:" << x3 << " " << y3 << std::endl;

        // Сортировка вершин по y
        if (y1 > y2) std::swap(x1, x2), std::swap(y1, y2);
        if (y1 > y3) std::swap(x1, x3), std::swap(y1, y3);
        if (y2 > y3) std::swap(x2, x3), std::swap(y2, y3);

        // std::cout << "SORT" << std::endl;
        // std::cout << "xy1:" << x1 << " " << y1 << std::endl;
        // std::cout << "xy2:" << x2 << " " << y2 << std::endl;
        // std::cout << "xy3:" << x3 << " " << y3 << std::endl;
        
        // Заполнение между двумя линиями
        for (int y = (int)y1 + 1; y <= (int)y3; ++y) {
            int x_start, x_end;

            // Вычисляем x_start и x_end для первой половины треугольника
            if (y < y2) {
                std::cout << "N";
                if(y1 != y2) {
                    x_start = (int)(x1 + (((x2 - x1) * (y - y1)) * 1.0f / (y2 - y1)));
                } else {
                    x_start = (int)x1;
                }
                if(y1 != y3) {
                    x_end = (int)(x1 + (((x3 - x1) * (y - y1)) * 1.0f / (y3 - y1)));
                } else {
                    x_end = (int)x1;
                }
            } else {
                x_start = (int)(x2 + (x3 - x2) * (y - y2) / (y3 - y2));
                x_end = (int)(x1 + (x3 - x1) * (y - y1) / (y3 - y1));
            }

            if(x_start > x_end) { std::swap(x_start, x_end); std::cout << "S"; }

            std::cout << x_start << " ::: " << x_end << std::endl;

            // Рисуем линию от x_start до x_end
            for (int x = x_start; x <= x_end; ++x) {
                if (x >= 0 && x < W && y >= 0 && y < H) {
                    ((uint32_t*)surface->pixels)[y * surface->pitch / 4 + x] = color;
                }
            }
        }
    }

    typedef struct {
        float x;
        float y;
        float w;
    } Point2D;

    void fillTriangleAlt(SDL_Surface* surface, const Matrix& v1, const Matrix& v2, const Matrix& v3, uint32_t color, Zbuffer& zbuffer) {
        float x1, y1, x2, y2, x3, y3, w1, w2, w3;
        worldToScreen(v1, x1, y1, w1);
        worldToScreen(v2, x2, y2, w2);
        worldToScreen(v3, x3, y3, w3);
        // Переведу все по удобству в объекты
        Point2D top, mid, bot;

        w1 = 1.0f / w1;
        w2 = 1.0f / w2;
        w3 = 1.0f / w3;

        top.x = x1;
        top.y = y1;
        top.w = w1;

        mid.x = x2;
        mid.y = y2;
        mid.w = w2;

        bot.x = x3;
        bot.y = y3;
        bot.w = w3;

        // Отсортируем вершины вертикально
        if(mid.y < top.y) {
            std::swap(mid, top);
        }

        if(bot.y < top.y) {
            std::swap(bot, top);
        }

        if(bot.y < mid.y) {
            std::swap(bot, mid);
        }

        float dyTopMid = mid.y - top.y;
        float dyTopBot = bot.y - top.y;
        float dyMidBot = bot.y - mid.y;
        
        // Проверяем, что треугольник имеет высоту больше 0 
        if(dyTopBot == 0) {
            return;
        }

        // Шаги от низа вверх
        Point2D topBotsStep;
        topBotsStep.x = ((bot.x - top.x) * 1.0f / dyTopBot);
        topBotsStep.w = ((bot.w - top.w) * 1.0f / dyTopBot);

        // Середина между вверхней и нижней вершиной
        Point2D mid2;
        mid2.x = top.x + dyTopMid * topBotsStep.x;
        mid2.y = top.y + dyTopMid;
        mid2.w = top.w + dyTopMid * topBotsStep.w;

        // Проверяем что mid левее mid2, так как мы будем рисовать горизонтальную линию слева-направо
        if(mid.x > mid2.x) {
            std::swap(mid, mid2);
        }

        if(dyTopMid) {
            Point2D leftStep;
            leftStep.x = ((mid.x - top.x) * 1.0f / dyTopMid);
            leftStep.w = ((mid.w - top.w) * 1.0f / dyTopMid);

            Point2D rightStep;
            rightStep.x = ((mid2.x - top.x) * 1.0f / dyTopMid);
            rightStep.w = ((mid2.w - top.w) * 1.0f / dyTopMid);

            float yStart = std::max(0.0f, top.y);
            float yEnd   = std::min(float(surface->h - 1.0f), mid.y);
            
            for(int y = yStart + 1; y < yEnd; y++) {
                float ySteps = y - top.y;

                Point2D left;
                left.x = std::trunc(top.x + ySteps * leftStep.x);
                left.w = top.w + ySteps * leftStep.w;

                Point2D right;
                right.x = std::trunc(top.x + ySteps * rightStep.x);
                right.w = top.w + ySteps * rightStep.w;

                // Рисуем линию слева направо
                float dx = right.x - left.x;
                if(dx != 0) {
                    float wStep = (right.w - left.w) * 1.0f / dx;
                    float xStart = std::max(0.0f, left.x);
                    float xEnd   = std::min(right.x, float(surface->w));

                    for(int x = xStart + 1; x < xEnd; x++) { 
                        float xSteps = x - left.x;
                        float w = left.w + xSteps * wStep;
                        float z = 1.0f / w;
                        
                        
                        if(z < zbuffer.getValue(x, y)) {
                            // std::cout << "(x, y): " << x << " " << y << " -- " << z << " vs " << zbuffer.getValue(x, y) << std::endl;
                            zbuffer.setValue(x, y, z);
                            // std::cout << " after " << zbuffer.getValue(x, y) << std::endl;
                        } else {
                            continue;
                        }
                        ((uint32_t*)surface->pixels)[y * surface->pitch / 4 + x] = color;
                    }
                }
            }
        }

        if(dyMidBot) {
            Point2D leftStep;
            leftStep.x = ((bot.x - mid.x) * 1.0f / dyMidBot);
            leftStep.w = ((bot.w - mid.w) * 1.0f / dyMidBot);

            Point2D rightStep;
            rightStep.x = ((bot.x - mid2.x) * 1.0f / dyMidBot);
            rightStep.w = ((bot.w - mid2.w) * 1.0f / dyMidBot);

            float yStart = std::max(0.0f, mid.y);
            float yEnd   = std::min(surface->h - 1.0f, bot.y);
            
            for(int y = yStart + 1; y < yEnd; y++) {
                float ySteps = y - mid.y;

                Point2D left;
                left.x = std::trunc(mid.x + ySteps * leftStep.x);
                left.w = mid.w + ySteps * leftStep.w;

                Point2D right;
                right.x = std::trunc(mid2.x + ySteps * rightStep.x);
                right.w = mid2.w + ySteps * rightStep.w;

                float dx = right.x - left.x;
                if(dx != 0) {
                    float wStep = (right.w - left.w) * 1.0f / dx;
                    float xStart = std::max(0.0f, left.x);
                    float xEnd   = std::min(right.x, float(surface->w));

                    for(int x = xStart + 1; x < xEnd; x++) { 
                        float xSteps = x - left.x;
                        float w = left.w + xSteps * wStep;
                        float z = 1.0f / w;

                        if(z < zbuffer.getValue(x, y)) {
                            zbuffer.setValue(x, y, z);
                        } else {
                            continue;
                        }
                        ((uint32_t*)surface->pixels)[y * surface->pitch / 4 + x] = color;
                    }
                }
            }
        }
    }


    // Отрисовка координатных осей
    void drawAxes(SDL_Surface* surface, Zbuffer zbuffer) {
        float axisLength = 2.0f;  // Длина осей

        Matrix origin(4, 1);
        origin.at(3, 0) = 1.0f;

        // Положительные оси
        Matrix xAxis(4, 1);
        xAxis.at(0, 0) = axisLength;
        xAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, xAxis, 0xFF0000, zbuffer);  // Красная ось X

        Matrix yAxis(4, 1);
        yAxis.at(1, 0) = axisLength;
        yAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, yAxis, 0x00FF00, zbuffer);  // Зеленая ось Y

        Matrix zAxis(4, 1);
        zAxis.at(2, 0) = axisLength;
        zAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, zAxis, 0x0000FF, zbuffer);  // Синяя ось Z

        // Отрицательные оси (более тусклые)
        Matrix negXAxis(4, 1);
        negXAxis.at(0, 0) = -axisLength;
        negXAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negXAxis, 0x800000, zbuffer);  // Темно-красная ось -X

        Matrix negYAxis(4, 1);
        negYAxis.at(1, 0) = -axisLength;
        negYAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negYAxis, 0x008000, zbuffer);  // Темно-зеленая ось -Y

        Matrix negZAxis(4, 1);
        negZAxis.at(2, 0) = -axisLength;
        negZAxis.at(3, 0) = 1.0f;
        drawLine(surface, origin, negZAxis, 0x000080, zbuffer);  // Темно-синяя ось -Z
    }

    void setScreenSize(int width, int height) {
        W = width;
        H = height;
        updateMatrices();
    }

    void rotate(float deltaYaw, float deltaPitch) {
        yaw += deltaYaw;
        pitch = std::max(float(-M_PI/2.0f + 0.1f), std::min(float(M_PI/2.0f - 0.1f), pitch + deltaPitch));
        updateMatrices();
    }

    void move(float forward, float right, float up) {
        // Вычисляем направление движения с учетом поворота камеры
        float cosYaw = cos(yaw);
        float sinYaw = sin(yaw);
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);
        
        // Движение вперед/назад с учетом pitch
        position.move(
            -forward * sinYaw * cosPitch,
            forward * sinPitch,
            -forward * cosYaw * cosPitch
        );
        
        // Движение вправо/влево (всегда горизонтальное)
        position.move(right * cosYaw, 0.0f, -right * sinYaw);
        
        // Движение вверх/вниз (всегда вертикальное)
        position.move(0.0f, up, 0.0f);
        
        updateMatrices();
    }

    // Получение позиции камеры
    Position3D getPosition() const {
        return position;
    }

};

#endif // _CAMERA_3D_HPP_