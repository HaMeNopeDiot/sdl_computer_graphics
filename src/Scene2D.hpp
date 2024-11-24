#ifndef _SCENE_2D_HPP_
#define _SCENE_2D_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Camera2D.hpp"
#include "Model2D.hpp"

class Scene2D {
private:
    std::vector<std::shared_ptr<Camera2D>> cameras;
    std::vector<std::shared_ptr<Model2D>> models;
    size_t activeCamera;  // Индекс активной камеры
    SDL_Window* win = nullptr;
    SDL_Surface* scr = nullptr;
public:
    Scene2D() : activeCamera(0) {
        // Создаем хотя бы одну камеру по умолчанию
        addCamera(std::make_shared<Camera2D>(1024, 1024));
    }

    void setWindow(SDL_Window* window) {
        win = window;
    }

    void setSurface(SDL_Surface* surface) {
        scr = surface;
    } 

    SDL_Window* getWindow() const {
        return win;
    }

    SDL_Surface* getSurface() const {
        return scr;
    }

    // Добавление камеры
    void addCamera(std::shared_ptr<Camera2D> camera) {
        cameras.push_back(camera);
    }

    // Добавление модели
    void addModel(std::shared_ptr<Model2D> model) {
        models.push_back(model);
    }

    // Получение активной камеры
    Camera2D& getActiveCamera() {
        return *cameras[activeCamera];
    }

    // Установка активной камеры
    void setActiveCamera(size_t index) {
        if (index < cameras.size()) {
            activeCamera = index;
        }
    }

    // Получение модели по индексу
    Model2D& getModel(size_t index) {
        return *models[index];
    }

    // Получение количества моделей
    size_t getModelCount() const {
        return models.size();
    }

    // Получение количества камер
    size_t getCameraCount() const {
        return cameras.size();
    }

    // Обновление размера экрана для всех камер
    void updateScreenSize(int width, int height) {
        for (auto& camera : cameras) {
            camera->updateScreenSize(width, height);
        }
    }

    // Отрисовка всех моделей через активную камеру
    void render() {
        if (cameras.empty() || models.empty()) return;

        // Очищаем экран
        SDL_FillRect(getSurface(), NULL, 0x11111111);  // Серый фон

        // Отрисовываем все модели через активную камеру
        for (const auto& model : models) {
            model->draw(getSurface(), *cameras[activeCamera]);
        }

        // Обновляем поверхность окна
        SDL_UpdateWindowSurface(win);
    }

    // Очистка сцены
    void clear() {
        models.clear();
        cameras.clear();
        activeCamera = 0;
    }
};

#endif // _SCENE_2D_HPP_
