#include <SDL2/SDL.h>
#include "Scene3D.hpp"
#include <cmath>
#include <memory>
#include <iostream>

bool jopa = false;

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
                        if(!jopa) { 
                            // Получаем текущие параметры куба
                            float scale_x = cube->get_model_size_X();
                            float scale_y = cube->get_model_size_Y();
                            float scale_z = cube->get_model_size_Z();

                            float xOffset = cube->getModelSizeXs() / 2.0f;
                            float yOffset = cube->getModelSizeYs() / 2.0f;
                            float zOffset = cube->getModelSizeZs() / 2.0f;
                            Position3D tmp_pos = cube->getPosition();
                            std::cout << "CENETER: " << tmp_pos.getX() << " " << tmp_pos.getY() << " " << tmp_pos.getZ() << "\n";

                            
                            // Переносим в центр координат и масштабируем к единичному размеру
                            cube->scale(1.0f / scale_x, 1.0f / scale_y, 1.0f / scale_z);
                            cube->translate(-tmp_pos.getX() + xOffset, 
                                            -tmp_pos.getY() - yOffset, 
                                            -tmp_pos.getZ() + zOffset);


                            
                            std::cout<< "Vertex Origin" << std::endl;
                            for(int i = 0; i < 8; i++) {
                                Position3D p_tmp = cube->getVertexPos(i);
                                std::cout << i << ": ";
                                p_tmp.printPos(xOffset, -yOffset, zOffset);
                            }

                            std::cout << "Vertex Transformed" << std::endl;
                            for(size_t i = 0; i < 8; i++) {
                                Position3D p_tmp = cube->getTransformedVertexPos(i);
                                std::cout << i << ": ";
                                p_tmp.printPos();
                            }

                            Position3D p1_origin = cube->getTransformedVertexPos(3);
                            p1_origin.printPos();
                            cube->translate(-p1_origin.getX(), -p1_origin.getY(), -p1_origin.getZ()); 
                            Position3D p2_origin = cube->getTransformedVertexPos(2);
                            p2_origin.printPos();

                            float p2X = p2_origin.getX();
                            float p2Y = p2_origin.getY();
                            float p2Z = p2_origin.getZ();
                            
                            
                            float AB = (sqrt(pow(p2X , 2) + pow(p2Y , 2) + pow(p2Z , 2)));

                            float cosAlpha = (p2Y * 1.0f) / (sqrt(pow(p2Z, 2) + pow(p2Y, 2)));
                            float sinAlpha = (p2Z * 1.0f) / (sqrt(pow(p2Z, 2) + pow(p2Y, 2)));

                            std::cout<< "COS: " << cosAlpha << "SIN: " << sinAlpha << '\n';

                            cube->rotateX(-cosAlpha, sinAlpha);
                            std::cout << "MY: ";
                            Position3D p3_origin = cube->getTransformedVertexPos(2);
                            p3_origin.printPos();

                            std::cout << "Vertex Transformed" << std::endl;
                            for(size_t i = 0; i < 8; i++) {
                                Position3D p_tmp = cube->getTransformedVertexPos(i);
                                std::cout << i << ": ";
                                p_tmp.printPos();
                            }

                            float p3X = p3_origin.getX();
                            float p3Y = p3_origin.getY();
                            float p3Z = p3_origin.getZ();

                            float cosBetta = (p3X * 1.0f) / (sqrt(pow(p3X, 2) + pow(p3Y, 2)));
                            float sinBetta = (p3Y * 1.0f) / (sqrt(pow(p3X, 2) + pow(p3Y, 2)));
                            
                            cube->rotateZ(cosBetta, -sinBetta);
                            
                            

                            lastMouseX = e.motion.x;
                            lastMouseY = e.motion.y;
                            std::cout << "MYX: ";
                            Position3D p4_origin = cube->getTransformedVertexPos(2);
                            p4_origin.printPos();

                            std::cout << "Vertex Transformed" << std::endl;
                            for(size_t i = 0; i < 8; i++) {
                                Position3D p_tmp = cube->getTransformedVertexPos(i);
                                std::cout << i << ": ";
                                p_tmp.printPos();
                            }

                            Position3D p0_origin = cube->getTransformedVertexPos(0);
                            float p0X = p0_origin.getX();
                            float p0Y = p0_origin.getY();
                            float p0Z = p0_origin.getZ();
                            
                            float cosGamma = (p0Y * 1.0f) / (sqrt(pow(p0Z, 2) + pow(p0Y, 2)));
                            float sinGamma = (p0Z * 1.0f) / (sqrt(pow(p0Z, 2) + pow(p0Y, 2)));

                            std::cout<< "COS: " << cosGamma << "SIN: " << sinGamma << '\n';

                            

                            std::cout << "Vertex Transformed" << std::endl;
                            for(size_t i = 0; i < 8; i++) {
                                Position3D p_tmp = cube->getTransformedVertexPos(i);
                                std::cout << i << ": ";
                                p_tmp.printPos();
                            }

                            cube->rotateX(-cosGamma, sinGamma);
                            jopa = false;

                            cube->rotateX(deltaX * 0.0025f);

                            cube->rotateX(-cosGamma, -sinGamma);
                            cube->rotateZ(cosBetta, sinBetta);
                            cube->rotateX(-cosAlpha, -sinAlpha);

                            cube->translate(p1_origin.getX(), p1_origin.getY(), p1_origin.getZ()); 
                            cube->translate(tmp_pos.getX() - xOffset, 
                                            tmp_pos.getY() + yOffset, 
                                            tmp_pos.getZ() - zOffset);
                            cube->scale(scale_x, scale_y, scale_z);
                        }
                        else {
                            // cube->rotateX(deltaX * 0.0025f);
                        }
                        
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