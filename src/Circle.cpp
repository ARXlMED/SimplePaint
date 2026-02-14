#include "Circle.hpp"
#include <cmath>
#include <algorithm>

Circle::Circle(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), baseRadius(radius) {}

std::vector<sf::Vector2f> Circle::getPoints() const {
    float r = baseRadius * scaleFactor;
    std::vector<sf::Vector2f> points;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2 * M_PI * i / segments;
        points.push_back(position + sf::Vector2f(r * std::cos(angle), r * std::sin(angle)));
    }
    return points;
}

void Circle::draw(sf::RenderWindow& window) const {
    float thickness = thicknesses[0];
    auto points = getPoints();
    for (int i = 0; i < segments; ++i) {
        drawThickLine(window, points[i], points[i+1], thickness, lineColor);
    }
    for (int i = 0; i < segments; ++i) {
        drawVertexCircle(window, points[i], thickness/2, lineColor);
    }
}

sf::FloatRect Circle::getBoundingBox() const {
    float r = baseRadius * scaleFactor;
    float maxThick = thicknesses[0];
    return sf::FloatRect(position.x - r - maxThick/2,
                         position.y - r - maxThick/2,
                         2*r + maxThick, 2*r + maxThick);
}

bool Circle::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Circle::scale(float factor) {
    Figure::scale(factor);
}

void Circle::setScale(float factor) {
    Figure::setScale(factor);
}