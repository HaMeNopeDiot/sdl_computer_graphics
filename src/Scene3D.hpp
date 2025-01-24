#ifndef _SCENE_3D_HPP_
#define _SCENE_3D_HPP_

#include "Camera3D.hpp"
#include "Model3D.hpp"
#include "HiddenSurfaceRemoval.hpp"
#include "Zbuffer.hpp"
#include <vector>
#include <memory>

class Scene3D {
private:
    std::vector<std::shared_ptr<Model3D>> models;
    Camera3D camera;
    SDL_Surface* surface;
    
    Zbuffer zbuffer;
    HiddenSurfaceRemoval hsr;    // Обработчик удаления невидимых поверхностей
    

public:
    Scene3D(int width, int height) 
        : camera(width, height),
          surface(SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0)),
          zbuffer(width, height)
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
        if (!targetSurface) return;

        // Очищаем поверхность светло-серым цветом
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x11, 0x11, 0x11));
        
        zbuffer.clear();
        // Отрисовка осей координат
        camera.drawAxes(surface, zbuffer);
        
        std::vector<uint32_t> model_colors;
        model_colors.push_back(0x006600);
        model_colors.push_back(0x660000);

        size_t indexModel = 0;
        // Отрисовка всех моделей
        for (auto& model : models) {
            // std::cout << "Model : " << indexModel << std::endl;
            model->draw(camera, surface, 0xFFFFFF, model_colors[indexModel++], zbuffer);  // Белый цвет для моделей
        }
        //getAllTransformedVerticies();
        

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
        zbuffer.clear();
    }

    Camera3D& getCamera() {
        return camera;
    }
    
    // Получение отсортированных видимых полигонов
    std::vector<Polygon3D> getVisiblePolygons(const Camera3D& camera) {
        auto sortedPolygons = hsr.sortPolygons();
        std::vector<Polygon3D> visiblePolygons;
        
        for (const auto& polygon : sortedPolygons) {
            if (hsr.isPolygonVisible(polygon, camera.getPosition())) {
                visiblePolygons.push_back(polygon);
            }
        }
        
        return visiblePolygons;
    }

    Matrix getAllTransformedVerticies()
    {
        size_t index = 0;
        
        std::vector<Matrix> modelsVerticies;
        for(auto& model : models) {
            std::cout<< "Matrix index - " << index++ << std::endl;
            Matrix TVerticies = model->getTransformedVertices();
            TVerticies.printMatrix();
            std::cout << std::endl;
            modelsVerticies.push_back(TVerticies);
        }

        // Merge all Verticies from models
        Matrix allInOneVerticies(4, 0);
        for(size_t i = 0; i < modelsVerticies.size(); i++) {
            allInOneVerticies = mergeVerticesERows(allInOneVerticies, modelsVerticies[i]);
        }

        allInOneVerticies.printMatrix();



        return Matrix(0, 0);
    }


    Matrix mergeVerticesERows(Matrix &A, Matrix &B) {
        size_t rowsA = A.getRows();
        size_t rowsB = B.getRows();

        size_t colsA = A.getCols();
        size_t colsB = B.getCols();

        if(rowsA == rowsB) {
            
            /* pass */
            size_t rows = rowsA;
            Matrix C(rows, colsA + colsB);

            for(size_t i = 0; i < rows; i++) {
                for(size_t j = 0; j < colsA; j++) {
                    C.at(i, j) = A.at(i, j);
                }
            }

            for(size_t i = 0; i < rows; i++) {
                for(size_t j = 0; j < colsB; j++) {
                    C.at(i, j + colsA) = B.at(i, j);
                }
            }
            // Our result
            return C;
        } else {
            /* wtf bro... */
            std::cout << rowsA << " and " << rowsB << "NOT EQUAL!" << std::endl;
            return Matrix(0, 0);
        }
    }
};



    

#endif /* _SCENE_3D_HPP_ */