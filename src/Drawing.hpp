#ifndef _DRAWING_HPP_
#define _DRAWING_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include <memory>

#include "Camera2D.hpp"


inline void drawLine(SDL_Surface* surface, Camera2D& camera, float x1, float y1, float x2, float y2, Uint32 color, float thickness = 1.0f) {
    // Convert world coordinates to screen coordinates
    int sx1, sy1, sx2, sy2;
    camera.worldToScreen(x1, y1, sx1, sy1);
    camera.worldToScreen(x2, y2, sx2, sy2);

    // Calculate line direction and perpendicular vector
    float dx = sx2 - sx1;
    float dy = sy2 - sy1;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length < 0.001f) return; // Prevent division by zero
    
    // Normalize direction vector
    float dirX = dx / length;
    float dirY = dy / length;
    
    // Calculate perpendicular vector
    float perpX = -dirY;
    float perpY = dirX;
    
    // Draw multiple parallel lines
    int halfThickness = static_cast<int>(thickness / 2);
    
    for (int i = -halfThickness; i <= halfThickness; i++) {
        // Calculate offset start and end points
        int offsetX1 = sx1 + static_cast<int>(perpX * i);
        int offsetY1 = sy1 + static_cast<int>(perpY * i);
        int offsetX2 = sx2 + static_cast<int>(perpX * i);
        int offsetY2 = sy2 + static_cast<int>(perpY * i);
        
        // Draw the line using Bresenham's algorithm
        int dx = abs(offsetX2 - offsetX1);
        int dy = abs(offsetY2 - offsetY1);
        int sx = (offsetX1 < offsetX2) ? 1 : -1;
        int sy = (offsetY1 < offsetY2) ? 1 : -1;
        int err = dx - dy;
        
        int x = offsetX1;
        int y = offsetY1;
        
        while (true) {
            if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
                *((Uint32*)surface->pixels + y * surface->w + x) = color;
            }
            
            if (x == offsetX2 && y == offsetY2) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }
}

inline void drawQuadraticCurve(SDL_Surface* surface, Camera2D& camera, float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color) {
    float lastX = x1;
    float lastY = y1;
    
    // Draw curve segments
    const int segments = 50;
    for (int i = 1; i <= segments; i++) {
        float t = static_cast<float>(i) / segments;
        
        // Quadratic Bézier formula
        float xt = (1-t)*(1-t)*x1 + 2*(1-t)*t*cx + t*t*x2;
        float yt = (1-t)*(1-t)*y1 + 2*(1-t)*t*cy + t*t*y2;
        
        // Draw line segment
        drawLine(surface, camera, lastX, lastY, xt, yt, color);
        
        lastX = xt;
        lastY = yt;
    }
}

#endif // _DRAWING_HPP_