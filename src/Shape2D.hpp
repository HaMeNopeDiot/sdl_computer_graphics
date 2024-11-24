#ifndef _SHAPE_2D_HPP_
#define _SHAPE_2D_HPP_

#include <SDL2/SDL.h>
#include "Drawing.hpp"
#include <cmath>

class Shape2D {
public:
    virtual ~Shape2D() = default;
    virtual void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) = 0;
};

class Line2D : public Shape2D {
private:
    float x1, y1, x2, y2;
    Uint32 color;
    float thickness;

public:
    Line2D(float x1, float y1, float x2, float y2, Uint32 color, float thickness = 1.0f)
        : x1(x1), y1(y1), x2(x2), y2(y2), color(color), thickness(thickness) {}

    void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) override;
};

class Curve2D : public Shape2D {
private:
    float x1, y1, cx, cy, x2, y2;
    Uint32 color;

public:
    Curve2D(float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color)
        : x1(x1), y1(y1), cx(cx), cy(cy), x2(x2), y2(y2), color(color) {}

    void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) ;
};

#endif // _SHAPE_2D_HPP_