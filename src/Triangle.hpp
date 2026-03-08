#pragma once
#include "PolylineFigure.hpp"

class Triangle : public PolylineFigure {
public:
    Triangle(float side, const sf::Color& color, const std::vector<float>& thicknesses);
};