#pragma once
#include "AbstractFigure.hpp"

class Circle : public AbstractFigure {
public:
    Circle(float radius, const sf::Color& color, float thickness);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    std::unique_ptr<AbstractFigure> clone() const override;

    float getOutlineThickness() const { return outlineThickness; }
    void setOutlineThickness(float thickness) { outlineThickness = thickness; }
    sf::Color getOutlineColor() const { return outlineColor; }
    void setOutlineColor(const sf::Color& color) { outlineColor = color; }

    void serialize(std::ostream& out) const override;
    void deserialize(std::istream& in) override;

    std::string getTypeName() const { return "Circle"; }
private:
    float baseRadius;
    sf::Color outlineColor;
    float outlineThickness;
};