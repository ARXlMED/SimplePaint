#include "Triangle.hpp"
#include <cmath>

Triangle::Triangle(float side, const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    float R = side / std::sqrt(3.f);
    vertices = {
        {0.f, -R},
        {R * 0.866f, R * 0.5f},
        {-R * 0.866f, R * 0.5f}
    };
}