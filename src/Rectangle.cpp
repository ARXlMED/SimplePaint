#include "Rectangle.hpp"

Rectangle::Rectangle(float width, float height, const sf::Color& color, const std::vector<float>& thicknesses)
    : PolylineFigure(color, thicknesses) {
    vertices = {
        {-width/2, -height/2},
        { width/2, -height/2},
        { width/2,  height/2},
        {-width/2,  height/2}
    };
}