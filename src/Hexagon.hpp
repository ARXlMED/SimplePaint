#pragma once
#include "PolylineFigure.hpp"

class Hexagon : public PolylineFigure {
public:
    Hexagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses);
};