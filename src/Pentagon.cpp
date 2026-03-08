#include "Pentagon.hpp"
#include <cmath>

Pentagon::Pentagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    for (int i = 0; i < 5; ++i) {
        float angle = i * 2 * M_PI / 5 - M_PI/2;
        vertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }
}