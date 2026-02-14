#pragma once
#include "Figure.hpp"

class Circle : public Figure {
public:
    Circle(float radius, const sf::Color& color, const std::vector<float>& thicknesses);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    void scale(float factor) override;
    void setScale(float factor) override;

private:
    float baseRadius;
    static constexpr int segments = 30;
    std::vector<sf::Vector2f> getPoints() const;
};