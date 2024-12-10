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
    cube->translate(0.0f, 0.0f, 0.0f);  // Отодвигаем куб от камеры
    scene.addModel(cube);

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
                            // Получаем текущие параметры куба
                            float scale_x = cube->get_model_size_X();
                            float scale_y = cube->get_model_size_Y();
                            float scale_z = cube->get_model_size_Z();

                            float xOffset = cube->getModelSizeXs() / 2.0f;
                            float yOffset = cube->getModelSizeYs() / 2.0f;
                            float zOffset = cube->getModelSizeZs() / 2.0f;
                            Position3D tmp_pos = cube->getPosition();

                            
                            // Переносим в центр координат и масштабируем к единичному размеру
                            cube->scale(1.0f / scale_x, 1.0f / scale_y, 1.0f / scale_z);
                            cube->translate(-tmp_pos.getX() + xOffset, 
                                            -tmp_pos.getY() - yOffset, 
                                            -tmp_pos.getZ() + zOffset);
                            // Положим веришну <3> в начало координат
                            Position3D p1_origin = cube->getTransformedVertexPos(3);
                            cube->translate(-p1_origin.getX(), -p1_origin.getY(), -p1_origin.getZ()); 

                            // Вычисляем угол поворота по Х
                            Position3D p2_origin = cube->getTransformedVertexPos(2);
                            
                            RotationAngle angleAlpha;
                            angleAlpha.calculateRotationAngleYZ(p2_origin);
                            float cosAlpha = angleAlpha.getCos();
                            float sinAlpha = angleAlpha.getSin();
                            // Осуществляем поворот по оси ОХ
                            cube->rotateX(-cosAlpha, sinAlpha);

                            // Вычисляем угол поворота по Z
                            Position3D p3_origin = cube->getTransformedVertexPos(2);
                            
                            RotationAngle angleBetta;
                            angleBetta.calculateRotationAngleXY(p3_origin);
                            float cosBetta = angleBetta.getCos();
                            float sinBetta = angleBetta.getSin();
                            // Осуществляем поворот по оси ОZ
                            cube->rotateZ(cosBetta, -sinBetta);
                            
                            // Вычисляем угол поворота по X
                            Position3D p0_origin = cube->getTransformedVertexPos(0);
                            
                            RotationAngle angleGamma;
                            angleGamma.calculateRotationAngleYZ(p0_origin);
                            float cosGamma = angleGamma.getCos();
                            float sinGamma = angleGamma.getSin();

                            // Осуществляем поворот по оси ОХ
                            cube->rotateX(-cosGamma, sinGamma);

                            // Поворачиваем куб вокруг своего ребра
                            cube->rotateX(deltaX * 0.0025f);

                            // Возвращаем куб в исходное положение по поворотам
                            cube->rotateX(-cosGamma, -sinGamma);
                            cube->rotateZ(cosBetta, sinBetta);
                            cube->rotateX(-cosAlpha, -sinAlpha);
                            // Возвращаем куб в исходное положение по координатам
                            cube->translate(p1_origin.getX(), p1_origin.getY(), p1_origin.getZ()); 
                            cube->translate(tmp_pos.getX() - xOffset, 
                                            tmp_pos.getY() + yOffset, 
                                            tmp_pos.getZ() - zOffset);
                            // Вовзращаем кубу исходный размер
                            cube->scale(scale_x, scale_y, scale_z);

                            
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