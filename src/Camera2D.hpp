#ifndef _CAMERA_2D_HPP_
#define _CAMERA_2D_HPP_

#include <SDL2/SDL.h>
#include "Position.hpp"

class Camera2D {
private:
    Position position;  // Camera position in normalized space
    float zoom;        // Camera zoom level
    int screenWidth;   // Screen width
    int screenHeight;  // Screen height
    float aspectRatio; // Screen aspect ratio

public:
    Camera2D(int width, int height) 
        : position(0.0f, 0.0f), zoom(1.0f), screenWidth(width), screenHeight(height) {
        aspectRatio = static_cast<float>(width) / height;
    }

    void move(float dx, float dy) {
        // Convert screen space movement to normalized space
        position.x += dx * 2.0f / screenWidth / zoom;
        position.y += dy * 2.0f / screenHeight / zoom;
    }

    void setPosition(float newX, float newY) {
        position.setPosition(newX, newY);
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
        screenWidth = width;
        screenHeight = height;
        aspectRatio = static_cast<float>(width) / height;
    }

    // Convert normalized coordinates to screen coordinates
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) {
        // Convert from normalized space (-1 to 1) to screen space
        float normalizedX = (worldX - position.x) * zoom;
        float normalizedY = (worldY - position.y) * zoom;
        
        // Apply aspect ratio correction
        normalizedX *= aspectRatio;
        
        // Convert to screen coordinates
        screenX = static_cast<int>((normalizedX + 1.0f) * screenWidth * 0.5f);
        screenY = static_cast<int>((normalizedY + 1.0f) * screenHeight * 0.5f);
    }

    // Convert screen coordinates to normalized coordinates
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) {
        // Convert from screen coordinates to normalized space
        float normalizedX = (screenX * 2.0f / screenWidth - 1.0f) / zoom;
        float normalizedY = (screenY * 2.0f / screenHeight - 1.0f) / zoom;
        
        // Remove aspect ratio correction
        normalizedX /= aspectRatio;
        
        // Add camera position
        worldX = normalizedX + position.x;
        worldY = normalizedY + position.y;
    }

    // Getters
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getAspectRatio() const { return aspectRatio; }
};

#endif // _CAMERA_2D_HPP_