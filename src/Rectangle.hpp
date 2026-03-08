#pragma once
#include "PolylineFigure.hpp"

class Rectangle : public PolylineFigure {
public:
    Rectangle(float width, float height, const sf::Color& color, const std::vector<float>& thicknesses);
};