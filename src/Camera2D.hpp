#ifndef _CAMERA_2D_HPP_
#define _CAMERA_2D_HPP_

#include <SDL2/SDL.h>
#include "Position.hpp"
#include <algorithm>

class Camera2D {
private:
    Position position;  // Camera position in normalized space
    float zoom;        // Camera zoom level
    int screenWidth;   // Screen width
    int screenHeight;  // Screen height
    float aspectRatio; // Screen aspect ratio

    // Нормализует значение в диапазоне [-1, 1] с учетом соотношения сторон
    float normalizeCoordinate(float value, bool isX) const {
        if (isX) {
            return value * std::min(1.0f, 1.0f / aspectRatio);
        } else {
            return value * std::min(1.0f, aspectRatio);
        }
    }

    // Денормализует значение из диапазона [-1, 1] с учетом соотношения сторон
    float denormalizeCoordinate(float value, bool isX) const {
        if (isX) {
            return value / std::min(1.0f, 1.0f / aspectRatio);
        } else {
            return value / std::min(1.0f, aspectRatio);
        }
    }

public:
    Camera2D(int width, int height) 
        : position(0.0f, 0.0f), zoom(1.0f), screenWidth(width), screenHeight(height) {
        updateAspectRatio();
    }

    void move(float dx, float dy) {
        // Convert screen space movement to normalized space
        float normalizedDx = dx * 2.0f / screenWidth / zoom;
        float normalizedDy = dy * 2.0f / screenHeight / zoom;
        
        position.x += normalizeCoordinate(normalizedDx, true);
        position.y += normalizeCoordinate(normalizedDy, false);
    }

    void setPosition(float newX, float newY) {
        position.setPosition(
            normalizeCoordinate(newX, true),
            normalizeCoordinate(newY, false)
        );
    }

    void setZoom(float newZoom) {
        if (newZoom > 0.0f) {
            zoom = newZoom;
        }
    }

    float getZoom() const {
        return zoom;
    }

    void updateScreenSize(int width, int height) {
        float oldAspect = aspectRatio;
        screenWidth = width;
        screenHeight = height;
        updateAspectRatio();

        // Корректируем позицию камеры при изменении соотношения сторон
        if (oldAspect != aspectRatio) {
            position.x = normalizeCoordinate(denormalizeCoordinate(position.x, true), true);
            position.y = normalizeCoordinate(denormalizeCoordinate(position.y, false), false);
        }
    }

    void updateAspectRatio() {
        aspectRatio = static_cast<float>(screenWidth) / screenHeight;
    }

    // Convert normalized coordinates to screen coordinates
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) {
        // Нормализуем координаты с учетом соотношения сторон
        float normalizedX = normalizeCoordinate(worldX - position.x, true) * zoom;
        float normalizedY = normalizeCoordinate(worldY - position.y, false) * zoom;

        // Convert to screen coordinates
        screenX = static_cast<int>((normalizedX + 1.0f) * screenWidth * 0.5f);
        screenY = static_cast<int>((normalizedY + 1.0f) * screenHeight * 0.5f);
    }

    // Convert screen coordinates to normalized coordinates
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) {
        // Convert from screen coordinates to normalized space
        float normalizedX = (screenX * 2.0f / screenWidth - 1.0f);
        float normalizedY = (screenY * 2.0f / screenHeight - 1.0f);
        
        // Apply zoom and denormalize coordinates
        normalizedX = denormalizeCoordinate(normalizedX / zoom, true);
        normalizedY = denormalizeCoordinate(normalizedY / zoom, false);
        
        // Add camera position
        worldX = normalizedX + position.x;
        worldY = normalizedY + position.y;
    }

    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getAspectRatio() const { return aspectRatio; }
};

#endif // _CAMERA_2D_HPP_