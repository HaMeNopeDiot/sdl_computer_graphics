#ifndef _SHAPE_2D_HPP_
#define _SHAPE_2D_HPP_

#include <SDL2/SDL.h>
#include "Drawing.hpp"
#include "Position.hpp"
#include <cmath>

class Shape2D {
public:
    virtual ~Shape2D() = default;
    virtual void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) = 0;
};

class Line2D : public Shape2D {
private:
    Position start;  // Начальная точка линии
    Position end;    // Конечная точка линии
    Uint32 color;
    float thickness;

public:
    Line2D(float x1, float y1, float x2, float y2, Uint32 color, float thickness = 1.0f)
        : start(x1, y1), end(x2, y2), color(color), thickness(thickness) {}

    Line2D(const Position& start, const Position& end, Uint32 color, float thickness = 1.0f)
        : start(start), end(end), color(color), thickness(thickness) {}

    void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) override;

    // Геттеры
    const Position& getStart() const { return start; }
    const Position& getEnd() const { return end; }
    Uint32 getColor() const { return color; }
    float getThickness() const { return thickness; }

    // Сеттеры
    void setStart(const Position& newStart) { start = newStart; }
    void setEnd(const Position& newEnd) { end = newEnd; }
    void setColor(Uint32 newColor) { color = newColor; }
    void setThickness(float newThickness) { thickness = newThickness; }
};

class Curve2D : public Shape2D {
private:
    Position start;    // Начальная точка кривой
    Position control;  // Контрольная точка
    Position end;      // Конечная точка кривой
    Uint32 color;

public:
    Curve2D(float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color)
        : start(x1, y1), control(cx, cy), end(x2, y2), color(color) {}

    Curve2D(const Position& start, const Position& control, const Position& end, Uint32 color)
        : start(start), control(control), end(end), color(color) {}

    void draw(SDL_Surface* surface, Camera2D& camera, float modelX, float modelY, float rotation, float scaleX, float scaleY) override;

    // Геттеры
    const Position& getStart() const { return start; }
    const Position& getControl() const { return control; }
    const Position& getEnd() const { return end; }
    Uint32 getColor() const { return color; }

    // Сеттеры
    void setStart(const Position& newStart) { start = newStart; }
    void setControl(const Position& newControl) { control = newControl; }
    void setEnd(const Position& newEnd) { end = newEnd; }
    void setColor(Uint32 newColor) { color = newColor; }
};

#endif // _SHAPE_2D_HPP_