#include <iostream>
#include <vector>
#include <limits>

struct Color {
    unsigned char r, g, b;
};

struct Pixel {
    float z;      // Глубина
    Color color;  // Цвет
};

class ZBuffer {
public:
    ZBuffer(int width, int height) : width(width), height(height) {
        // Инициализация Z-буфера
        zBuffer.resize(height, std::vector<float>(width, std::numeric_limits<float>::infinity()));
        frameBuffer.resize(height, std::vector<Color>(width));
    }

    void clear() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                zBuffer[y][x] = std::numeric_limits<float>::infinity();
                frameBuffer[y][x] = {0, 0, 0}; // Черный цвет фона
            }
        }
    }

    void putTriangle(float vertices[3][2], float depth[3], Color color) {
        // Преобразование треугольника в растровую форму и заполнение Z-буфера
        for (int i = 0; i < 3; ++i) {
            int x = static_cast<int>(vertices[i][0]);
            int y = static_cast<int>(vertices[i][1]);
            if (x >= 0 && x < width && y >= 0 && y < height) {
                if (depth[i] < zBuffer[y][x]) {
                    zBuffer[y][x] = depth[i];
                    frameBuffer[y][x] = color;
                }
            }
        }
    }

    void show() const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Color& c = frameBuffer[y][x];
                std::cout << "(" << static_cast<int>(c.r) << ", "
                          << static_cast<int>(c.g) << ", "
                          << static_cast<int>(c.b) << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    int width, height;
    std::vector<std::vector<float>> zBuffer;
    std::vector<std::vector<Color>> frameBuffer;
};

int main() {
    ZBuffer zb(800, 600);
    zb.clear();

    // Пример треугольника
    float vertices[3][2] = {{100.0f, 100.0f}, {200.0f, 100.0f}, {150.0f, 200.0f}};
    float depths[3] = {0.5f, 0.5f, 0.5f}; // Глубина треугольника
    Color color = {255, 0, 0}; // Красный цвет

    zb.putTriangle(vertices, depths, color);
    
    // Отображение результата
    zb.show();

    return 0;
}
