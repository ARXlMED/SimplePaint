#pragma once
#include "Figure.hpp"

class Triangle : public Figure {
public:
    Triangle(float side, const sf::Color& color, const std::vector<float>& thicknesses);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    void scale(float factor) override;
    void setScale(float factor) override;

private:
    float baseSide;
    std::vector<sf::Vector2f> getLocalVertices() const;
    std::vector<sf::Vector2f> getGlobalVertices() const;
};