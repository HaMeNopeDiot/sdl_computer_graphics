
#include <SDL2/SDL.h>


#include "Camera2D.hpp"
#include "Model2D.hpp"
#include <cmath>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;

SDL_Window *win = NULL;
SDL_Surface *scr = NULL;
SDL_Surface *smile = NULL;
Camera2D camera(SCREEN_WIDTH, SCREEN_HEIGHT);
Model2D model;

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    win = SDL_CreateWindow("My window", 
                          SDL_WINDOWPOS_UNDEFINED, 
                          SDL_WINDOWPOS_UNDEFINED, 
                          SCREEN_WIDTH, 
                          SCREEN_HEIGHT, 
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (win == NULL) {
        return 1;
    }

    scr = SDL_GetWindowSurface(win);

    return 0;
}

int load() {
    smile = SDL_LoadBMP("yoimya.bmp");

    if (smile == NULL) {
        return 1;
    }

    return 0;
}

void quit() {
    SDL_FreeSurface(smile);
    
    SDL_DestroyWindow(win);

    SDL_Quit();
}



void redraw_window(SDL_Surface* surface, Camera2D& cur_camera, Model2D& model) {
    SDL_FillRect(surface, NULL, 0x000000);  // Clear with black
    // Draw the model
    model.draw(surface, cur_camera);
    
    SDL_UpdateWindowSurface(win);
}

int main (int argc, char ** args) {
    if (init() == 1) {
        return 1;
    }

    // Create a model with multiple shapes
    Model2D model;
    model.addLine(-1.0f, -1.0f, -0.5f, -0.5f, 0x0000FF);     // Blue line
    model.addLine(-0.5f, -0.5f, 0.5f, 0.5f, 0xFF0000, 3.0f); // Red thick line
    model.addLine(0.5f, 0.5f, 1.0f, 1.0f, 0xFFFF00);         // Yellow line
    model.addCurve(-0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.0f, 0xFFFF00); // Yellow curve

    // Initial draw
    redraw_window(scr, camera, model);

    SDL_Event e;
    bool quit_check = false;
    bool isDragging = false;
    int lastMouseX = 0, lastMouseY = 0;
    
    while (!quit_check) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit_check = true;
            }
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    scr = SDL_GetWindowSurface(win);
                    int newWidth, newHeight;
                    SDL_GetWindowSize(win, &newWidth, &newHeight);
                    camera.updateScreenSize(newWidth, newHeight);
                    redraw_window(scr, camera, model);
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
                camera.move(-deltaX, -deltaY);
                lastMouseX = e.motion.x;
                lastMouseY = e.motion.y;
                redraw_window(scr, camera, model);
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                float zoomFactor = (e.wheel.y > 0) ? 1.1f : 0.9f;
                camera.setZoom(camera.getZoom() * zoomFactor);
                redraw_window(scr, camera, model);
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w: camera.move(0, -10); break;
                    case SDLK_s: camera.move(0, 10); break;
                    case SDLK_a: camera.move(-10, 0); break;
                    case SDLK_d: camera.move(10, 0); break;
                    case SDLK_q: camera.setZoom(camera.getZoom() * 1.1f); break;
                    case SDLK_e: camera.setZoom(camera.getZoom() / 1.1f); break;
                }
                redraw_window(scr, camera, model);
            }
        }
    }

    quit();
    return 0;
}