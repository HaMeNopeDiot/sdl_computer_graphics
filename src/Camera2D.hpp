#ifndef _CAMERA_2D_HPP_
#define _CAMERA_2D_HPP_

#pragma once
#include <SDL2/SDL.h>

class Camera2D {
private:
    float x, y;        // Camera position in normalized space
    float zoom;        // Camera zoom level
    int screenWidth;   // Screen width
    int screenHeight;  // Screen height
    float aspectRatio; // Screen aspect ratio

public:
    Camera2D(int width, int height) 
        : x(0.0f), y(0.0f), zoom(1.0f), screenWidth(width), screenHeight(height) {
        aspectRatio = static_cast<float>(width) / height;
    }

    void move(float dx, float dy) {
        // Convert screen space movement to normalized space
        x += dx * 2.0f / screenWidth / zoom;
        y += dy * 2.0f / screenHeight / zoom;
    }

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void setZoom(float newZoom) {
        if (newZoom > 0.0f) {
            zoom = newZoom;
        }
    }

    // Convert normalized coordinates to screen coordinates
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) {
        // Convert from normalized space (-1 to 1) to screen space
        float normalizedX = (worldX - x) * zoom;
        float normalizedY = (worldY - y) * zoom;
        
        // Apply aspect ratio correction
        normalizedX *= aspectRatio;
        
        // Convert to screen coordinates
        screenX = static_cast<int>((normalizedX + 1.0f) * screenWidth * 0.5f);
        screenY = static_cast<int>((normalizedY + 1.0f) * screenHeight * 0.5f);
    }

    // Convert screen coordinates to normalized coordinates
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) {
        // Convert from screen space to normalized space
        worldX = (screenX * 2.0f / screenWidth - 1.0f) / zoom + x;
        worldY = (screenY * 2.0f / screenHeight - 1.0f) / zoom + y;
        
        // Apply aspect ratio correction
        worldX /= aspectRatio;
    }

    void updateScreenSize(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        aspectRatio = static_cast<float>(width) / height;
    }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getZoom() const { return zoom; }
    float getAspectRatio() const { return aspectRatio; }
};

#endif // _CAMERA_2D_HPP_