#pragma once
#include "PolylineFigure.hpp"

class Circle : public PolylineFigure {
public:
    Circle(float radius, const sf::Color& color, const std::vector<float>& thicknesses);
};