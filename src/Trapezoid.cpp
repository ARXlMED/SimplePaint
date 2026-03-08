#include "Trapezoid.hpp"

Trapezoid::Trapezoid(float topBase, float bottomBase, float height,
                     const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    vertices = {
        { -bottomBase/2, -height/2 },
        {  bottomBase/2, -height/2 },
        {  topBase/2,     height/2 },
        { -topBase/2,     height/2 }
    };
}