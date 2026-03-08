#pragma once
#include "PolylineFigure.hpp"

class Pentagon : public PolylineFigure {
public:
    Pentagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses);
};