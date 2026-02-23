#pragma once
#include "Figure.hpp"

class Rectangle : public Figure {
public:
    Rectangle(float width, float height, const sf::Color& color,
              const std::vector<float>& thicknesses);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    void scale(float factor) override;
    void setScale(float factor) override;

private:
    // Больше не храним baseWidth/baseHeight, они в baseVertices
};