#include <SDL2/SDL.h>
#include "Scene2D.hpp"
#include "Mesh.hpp"
#include <cmath>
#include <memory>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

// Глобальные переменные для управления выбранной фигурой
std::shared_ptr<Shape2D> selectedShape = nullptr;
size_t selectedShapeIndex = 0;
size_t selectedModelIndex = 0;

// Создание модели из меша
Model2D createModelFromMesh(const Mesh& mesh, uint32_t color = 0xFFFFFF, float lineWidth = 1.0f) {
    Model2D model;
    
    // Создаем линии из ребер меша
    const auto& edges = mesh.getEdges();
    const auto& vertices = mesh.getVertices();
    
    for (const auto& edge : edges) {
        const Position& v1 = vertices[edge.vertex1];
        const Position& v2 = vertices[edge.vertex2];
        auto line = std::make_shared<Line2D>(v1, v2, color, lineWidth);
        model.addShape(line);
    }
    
    return model;
}

// Создание квадрата через меш
Mesh createSquareMesh(float size) {
    Mesh mesh;
    
    // Добавляем вершины
    int topLeft = mesh.addVertex(-size, -size);
    int topRight = mesh.addVertex(size, -size);
    int bottomRight = mesh.addVertex(size, size);
    int bottomLeft = mesh.addVertex(-size, size);
    
    // Добавляем ребра
    mesh.addEdge(topLeft, topRight);
    mesh.addEdge(topRight, bottomRight);
    mesh.addEdge(bottomRight, bottomLeft);
    mesh.addEdge(bottomLeft, topLeft);
    
    return mesh;
}

// Создание треугольника через меш
Mesh createTriangleMesh(float size) {
    Mesh mesh;
    
    // Добавляем вершины
    int top = mesh.addVertex(0, -size);
    int bottomRight = mesh.addVertex(size, size);
    int bottomLeft = mesh.addVertex(-size, size);
    
    // Добавляем ребра
    mesh.addEdge(top, bottomRight);
    mesh.addEdge(bottomRight, bottomLeft);
    mesh.addEdge(bottomLeft, top);
    
    return mesh;
}

// Выбор следующей фигуры в текущей модели
void selectNextShape(Model2D& model) {
    if (model.getShapeCount() == 0) return;
    
    selectedShapeIndex = (selectedShapeIndex + 1) % model.getShapeCount();
    selectedShape = model.getShape(selectedShapeIndex);
}

// Выбор предыдущей фигуры в текущей модели
void selectPrevShape(Model2D& model) {
    if (model.getShapeCount() == 0) return;
    
    if (selectedShapeIndex == 0) {
        selectedShapeIndex = model.getShapeCount() - 1;
    } else {
        selectedShapeIndex--;
    }
    selectedShape = model.getShape(selectedShapeIndex);
}

int main(int argc, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "2D Graphics",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Создаем и инициализируем сцену
    Scene2D scene;
    scene.setWindow(window);
    scene.setSurface(surface);

    // Создаем модели из мешей
    auto squareMesh = createSquareMesh(0.3f);
    auto squareModel = std::make_shared<Model2D>(createModelFromMesh(squareMesh, 0xFFFFFF, 1.0f)); // Белый квадрат
    scene.addModel(squareModel);

    auto triangleMesh = createTriangleMesh(0.4f);
    auto triangleModel = std::make_shared<Model2D>(createModelFromMesh(triangleMesh, 0xFF0000, 2.0f)); // Красный треугольник
    scene.addModel(triangleModel);

    // Начальные позиции моделей
    scene.getModel(0).setPosition(-0.5f, 0.0f);  // Квадрат слева
    scene.getModel(1).setPosition(0.5f, 0.0f);   // Треугольник справа

    // Выбираем первую фигуру первой модели
    if (scene.getModelCount() > 0 && scene.getModel(0).getShapeCount() > 0) {
        selectedModelIndex = 0;
        selectedShape = scene.getModel(0).getShape(0);
    }

    // Initial draw
    scene.render();

    SDL_Event e;
    bool quit = false;
    bool isDragging = false;
    int lastMouseX = 0, lastMouseY = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    surface = SDL_GetWindowSurface(window);
                    scene.setSurface(surface);
                    int newWidth, newHeight;
                    SDL_GetWindowSize(window, &newWidth, &newHeight);
                    scene.updateScreenSize(newWidth, newHeight);
                    scene.render();
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isDragging = true;
                    lastMouseX = e.button.x;
                    lastMouseY = e.button.y;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isDragging = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION && isDragging) {
                int deltaX = e.motion.x - lastMouseX;
                int deltaY = e.motion.y - lastMouseY;
                scene.getActiveCamera().move(-deltaX, -deltaY);
                lastMouseX = e.motion.x;
                lastMouseY = e.motion.y;
                scene.render();
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                float zoomFactor = (e.wheel.y > 0) ? 1.1f : 0.9f;
                scene.getActiveCamera().setZoom(scene.getActiveCamera().getZoom() * zoomFactor);
                scene.render();
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    // Управление камерой
                    case SDLK_w: scene.getActiveCamera().move(0, -10); break;
                    case SDLK_s: scene.getActiveCamera().move(0, 10); break;
                    case SDLK_a: scene.getActiveCamera().move(-10, 0); break;
                    case SDLK_d: scene.getActiveCamera().move(10, 0); break;

                    // Выбор модели и фигуры
                    case SDLK_TAB:
                        if (SDL_GetModState() & KMOD_SHIFT) {
                            if (scene.getModelCount() > 0) {
                                selectedModelIndex = (selectedModelIndex + 1) % scene.getModelCount();
                                if (scene.getModel(selectedModelIndex).getShapeCount() > 0) {
                                    selectedShape = scene.getModel(selectedModelIndex).getShape(0);
                                    selectedShapeIndex = 0;
                                }
                            }
                        } else {
                            if (scene.getModelCount() > 0) {
                                selectNextShape(scene.getModel(selectedModelIndex));
                            }
                        }
                        break;

                    // Управление моделью
                    case SDLK_LEFT:
                        if (scene.getModelCount() > 0) {
                            scene.getModel(selectedModelIndex).move(-0.1f, 0.0f);
                        }
                        break;
                    case SDLK_RIGHT:
                        if (scene.getModelCount() > 0) {
                            scene.getModel(selectedModelIndex).move(0.1f, 0.0f);
                        }
                        break;
                    case SDLK_UP:
                        if (scene.getModelCount() > 0) {
                            scene.getModel(selectedModelIndex).move(0.0f, -0.1f);
                        }
                        break;
                    case SDLK_DOWN:
                        if (scene.getModelCount() > 0) {
                            scene.getModel(selectedModelIndex).move(0.0f, 0.1f);
                        }
                        break;
                    case SDLK_q:
                        if (SDL_GetModState() & KMOD_SHIFT) {
                            if (scene.getModelCount() > 0) {
                                scene.getModel(selectedModelIndex).setRotation(
                                    scene.getModel(selectedModelIndex).getRotation() - 5.0f
                                );
                            }
                        } else {
                            scene.getActiveCamera().setZoom(scene.getActiveCamera().getZoom() * 1.1f);
                        }
                        break;
                    case SDLK_r:
                        if (SDL_GetModState() & KMOD_SHIFT) {
                            if (scene.getModelCount() > 0) {
                                scene.getModel(selectedModelIndex).setRotation(
                                    scene.getModel(selectedModelIndex).getRotation() + 5.0f
                                );
                            }
                        } else {
                            scene.getActiveCamera().setZoom(scene.getActiveCamera().getZoom() / 1.1f);
                        }
                        break;
                }
                scene.render();
            }
        }
    }

    // Очистка ресурсов
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}