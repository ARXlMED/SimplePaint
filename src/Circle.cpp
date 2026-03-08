#include "Circle.hpp"
#include <cmath>

Circle::Circle(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    const int segments = 30;
    for (int i = 0; i < segments; ++i) {
        float angle = 2 * M_PI * i / segments;
        vertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }
}