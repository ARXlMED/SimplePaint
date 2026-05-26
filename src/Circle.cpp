#include "Circle.hpp"
#include <cmath>


Circle::Circle(float radius, const sf::Color& color, float thickness)
    : baseRadius(radius), outlineColor(color), outlineThickness(thickness) {}

void Circle::draw(sf::RenderWindow& window) const {
    float r = baseRadius * scaleFactor;
    int pointCount = static_cast<int>(r * 5);
    sf::CircleShape circle(r, pointCount);
    circle.setOrigin(r, r);
    circle.setPosition(position);
    circle.setFillColor(filled ? fillColor : sf::Color::Transparent);
    circle.setOutlineColor(outlineColor);
    circle.setOutlineThickness(outlineThickness);
    window.draw(circle);
}

bool Circle::contains(const sf::Vector2f& point) const {
    float r = baseRadius * scaleFactor + outlineThickness * 0.5f;
    float dx = point.x - position.x;
    float dy = point.y - position.y;
    return dx*dx + dy*dy <= r*r;
}

sf::FloatRect Circle::getBoundingBox() const {
    float r = baseRadius * scaleFactor + outlineThickness;
    return sf::FloatRect(position.x - r, position.y - r, 2*r, 2*r);
}

std::unique_ptr<AbstractFigure> Circle::clone() const {
    auto copy = std::make_unique<Circle>(baseRadius, outlineColor, outlineThickness);
    copy->setPosition(position);
    copy->setScale(scaleFactor);
    copy->setFillColor(fillColor);
    copy->setFilled(filled);
    copy->setLocalPivot(pivot);
    return copy;
}

void Circle::serialize(std::ostream& out) const {
    AbstractFigure::serialize(out);  // пишет тип и общие поля
    out << baseRadius << ' '
        << (int)outlineColor.r << ' ' << (int)outlineColor.g << ' ' << (int)outlineColor.b << ' '
        << outlineThickness << '\n';
}

void Circle::deserialize(std::istream& in) {
    AbstractFigure::deserialize(in);
    int r,g,b;
    in >> baseRadius >> r >> g >> b >> outlineThickness;
    outlineColor = sf::Color(r,g,b);
}