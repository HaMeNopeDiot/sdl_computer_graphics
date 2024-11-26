#pragma once
#include "Camera3D.hpp"
#include "Model3D.hpp"
#include <vector>
#include <memory>

class Scene3D {
private:
    std::vector<std::shared_ptr<Model3D>> models;
    Camera3D camera;
    SDL_Surface* surface;

public:
    Scene3D(int width, int height) 
        : camera(width, height),
          surface(SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0))
    {
    }

    ~Scene3D() {
        if (surface) {
            SDL_FreeSurface(surface);
        }
    }

    // Добавление модели в сцену
    void addModel(std::shared_ptr<Model3D> model) {
        models.push_back(model);
    }

    // Очистка экрана
    void clear(uint32_t color = 0) {
        SDL_FillRect(surface, nullptr, color);
    }

    // Отрисовка всей сцены
    void render(SDL_Surface* targetSurface) {
        clear();
        
        // Отрисовка осей координат
        camera.drawAxes(surface);
        
        // Отрисовка всех моделей
        for (auto& model : models) {
            model->draw(camera, surface, 0xFFFFFF);  // Белый цвет для моделей
        }
        
        // Копируем результат на целевую поверхность
        SDL_BlitSurface(surface, nullptr, targetSurface, nullptr);
    }

    // Изменение размера окна
    void resize(int width, int height) {
        if (surface) {
            SDL_FreeSurface(surface);
        }
        surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
        camera.setScreenSize(width, height);
    }
};
