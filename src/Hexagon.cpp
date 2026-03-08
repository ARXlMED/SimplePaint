#include "Hexagon.hpp"
#include <cmath>

Hexagon::Hexagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    for (int i = 0; i < 6; ++i) {
        float angle = i * 2 * M_PI / 6 - M_PI/2;
        vertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }
}