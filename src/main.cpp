#include <SDL2/SDL.h>
#include "Scene3D.hpp"
#include <cmath>
#include <memory>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

int main(int argc, char** args) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "3D Graphics",
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
    Scene3D scene(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Создаем куб
    auto cube = Model3D::createCube(0.5f);
    cube->translate(0.0f, 0.0f, 0.0f);  // Отодвигаем куб от камеры
    scene.addModel(cube);

    // Initial draw
    scene.render(surface);
    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    bool isDragging = false;
    int lastMouseX = 0, lastMouseY = 0;
    float rotationX = 0.0f, rotationY = 0.0f;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    surface = SDL_GetWindowSurface(window);
                    int newWidth, newHeight;
                    SDL_GetWindowSize(window, &newWidth, &newHeight);
                    scene.resize(newWidth, newHeight);
                    scene.render(surface);
                    SDL_UpdateWindowSurface(window);
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
                
                // Готовимся к вращению
                rotationY += deltaX * 0.01f;
                rotationX += deltaY * 0.01f;
                
                cube->applyTransform(Matrix::identity(4));  // Сбрасываем трансформации

                Position3D tmp_pos = cube->getPosition();
                // Центрируем модель
                cube->translate(-tmp_pos.getX(), -tmp_pos.getY(), -tmp_pos.getZ());        // Отодвигаем от камеры
                // Вращаем модель
                cube->rotateX(rotationX);                  // Применяем поворот по X
                cube->rotateY(rotationY);                  // Применяем поворот по Y
                // Возвращаем модель
                cube->translate(tmp_pos.getX(), tmp_pos.getY(), tmp_pos.getZ());
                
                lastMouseX = e.motion.x;
                lastMouseY = e.motion.y;
                
                scene.render(surface);
                SDL_UpdateWindowSurface(window);
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        cube->translate(0.0f, -0.1f, 0.0f);
                        break;
                    case SDLK_s:
                        cube->translate(0.0f, 0.1f, 0.0f);
                        break;
                    case SDLK_a:
                        cube->translate(-0.1f, 0.0f, 0.0f);
                        break;
                    case SDLK_d:
                        cube->translate(0.1f, 0.0f, 0.0f);
                        break;
                    case SDLK_q:
                        cube->translate(0.0f, 0.0f, 0.1f);
                        break;
                    case SDLK_e:
                        cube->translate(0.0f, 0.0f, -0.1f);
                        break;
                }
                scene.render(surface);
                SDL_UpdateWindowSurface(window);
            }
        }
    }

    // Очистка ресурсов
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}