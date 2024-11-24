#include "Model2D.hpp"

void Model2D::addLine(float x1, float y1, float x2, float y2, Uint32 color, float thickness) {
    shapes.push_back(std::make_shared<Line2D>(x1, y1, x2, y2, color, thickness));
}

void Model2D::addCurve(float x1, float y1, float cx, float cy, float x2, float y2, Uint32 color) {
    shapes.push_back(std::make_shared<Curve2D>(x1, y1, cx, cy, x2, y2, color));
}

void Model2D::draw(SDL_Surface* surface, Camera2D& camera) {
    for (const auto& shape : shapes) {
        shape->draw(surface, camera, posX, posY, rotation, scaleX, scaleY);
    }
}
