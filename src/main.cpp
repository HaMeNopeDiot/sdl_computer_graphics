#include <SDL2/SDL.h>
#include "Scene3D.hpp"
#include <cmath>
#include <memory>
#include <iostream>

#include "RotationAngle.hpp"

bool debug = false;

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
    auto triangle = Model3D::createTriangle(0.5f);
    
    triangle->translate(1.0f, 0.0f, 0.0f);
    cube->translate(0.0f, 0.0f, 0.0f);  // Отодвигаем куб от камеры
    scene.addModel(cube);
    scene.addModel(triangle);

    // Initial draw
    scene.render(surface);
    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    bool isLeftDragging = false;   // Для вращения куба
    bool isRightDragging = false;  // Для управления камерой
    bool isShiftPressed = false;
    int lastMouseX = 0, lastMouseY = 0;

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
                    isLeftDragging = true;
                    lastMouseX = e.button.x;
                    lastMouseY = e.button.y;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    isRightDragging = true;
                    lastMouseX = e.button.x;
                    lastMouseY = e.button.y;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    isLeftDragging = false;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    isRightDragging = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                int deltaX = e.motion.x - lastMouseX;
                int deltaY = e.motion.y - lastMouseY;

                if (isLeftDragging) {
                    if(isShiftPressed) {
                        if(!debug) { 
                            cube->RotateOverEdge(2, 3, deltaX);

                            debug = false;
                        }
                        
                        lastMouseX = e.motion.x;
                        lastMouseY = e.motion.y;

                        
                        scene.render(surface);
                        SDL_UpdateWindowSurface(window);
                    } else {
                        // cube->applyTransform(Matrix::identity(4));  // Сбрасываем трансформации
                        Position3D tmp_pos = cube->getPosition();
                        // Центрируем модель
                        cube->translate(-tmp_pos.getX(), -tmp_pos.getY(), -tmp_pos.getZ());
                        // Вращаем модель
                        cube->rotateY(deltaX * 0.0025f);
                        cube->rotateX(deltaY * 0.0025f);
                        // Возвращаем модель
                        cube->translate(tmp_pos.getX(), tmp_pos.getY(), tmp_pos.getZ());
                        
                        // std::cout << "X:" << cube->getRotationX() << " Y:" << cube->getRotationY() << " Z:" << cube->getRotationZ() << "\n";
                        
                        lastMouseX = e.motion.x;
                        lastMouseY = e.motion.y;
                        
                        scene.render(surface);
                        SDL_UpdateWindowSurface(window);
                    }
                }
                else if (isRightDragging) {
                    // Вращение камеры
                    scene.getCamera().rotate(deltaX * 0.01f, deltaY * 0.01f);
                    
                    lastMouseX = e.motion.x;
                    lastMouseY = e.motion.y;
                    
                    scene.render(surface);
                    SDL_UpdateWindowSurface(window);
                }
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                float scale_factor = (e.wheel.y > 0) ? 1.1f : 0.9f;
                cube->applyTransform(Matrix::identity(4));
                Position3D tmp_pos = cube->getPosition();

                cube->translate(-tmp_pos.getX(), -tmp_pos.getY(), -tmp_pos.getZ());
                cube->scale(scale_factor, scale_factor, scale_factor);
                cube->translate(tmp_pos.getX(), tmp_pos.getY(), tmp_pos.getZ());
                
                scene.render(surface);
                SDL_UpdateWindowSurface(window);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_LSHIFT) {
                    isShiftPressed = true;
                }
                float moveSpeed = 0.25f;
                bool moved = false;
                float forward = 0.0f, right = 0.0f, up = 0.0f;

                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        forward = moveSpeed;
                        moved = true;
                        break;
                    case SDLK_DOWN:
                        forward = -moveSpeed;
                        moved = true;
                        break;
                    case SDLK_LEFT:
                        right = -moveSpeed;
                        moved = true;
                        break;
                    case SDLK_RIGHT:
                        right = moveSpeed;
                        moved = true;
                        break;
                    case SDLK_PAGEUP:
                        up = moveSpeed;
                        moved = true;
                        break;
                    case SDLK_PAGEDOWN:
                        up = -moveSpeed;
                        moved = true;
                        break;
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

                if (moved) {
                    scene.getCamera().move(forward, right, up);
                    scene.render(surface);
                    SDL_UpdateWindowSurface(window);
                }
                else {
                    scene.render(surface);
                    SDL_UpdateWindowSurface(window);
                }
            }
            else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_LSHIFT) {
                    isShiftPressed = false;
                }
            }
        }
    }

    // Очистка ресурсов
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}