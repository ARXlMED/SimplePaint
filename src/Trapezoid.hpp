#pragma once
#include "PolylineFigure.hpp"

class Trapezoid : public PolylineFigure {
public:
    Trapezoid(float topBase, float bottomBase, float height,
              const sf::Color& color, const std::vector<float>& thicknesses);
};