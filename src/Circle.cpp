#include "Circle.hpp"
#include <cmath>
#include <algorithm>

Circle::Circle(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses) {
    baseVertices.clear();
    for (int i = 0; i < segments; ++i) {
        float angle = 2 * M_PI * i / segments;
        baseVertices.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }
}

std::vector<sf::Vector2f> Circle::getPoints() const {
    std::vector<sf::Vector2f> points;
    for (const auto& v : baseVertices)
        points.push_back(position + v * scaleFactor);
    points.push_back(position + baseVertices[0] * scaleFactor);
    return points;
}

void Circle::draw(sf::RenderWindow& window) const {
    float radius = 0;
    if (!baseVertices.empty())
        radius = std::sqrt(baseVertices[0].x * baseVertices[0].x + baseVertices[0].y * baseVertices[0].y) * scaleFactor;

    if (filled) {
        sf::CircleShape circle(radius);
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setPosition(position);
        circle.setFillColor(fillColor);
        window.draw(circle);
    }

    auto points = getPoints();
    int n = segments;
    float thick = thicknesses[0];
    for (int i = 0; i < n; ++i) {
        drawThickLine(window, points[i], points[i+1], thick, sideColors[0]);
    }
    for (int i = 0; i < n; ++i) {
        drawVertexCircle(window, points[i], thick/2, sideColors[0]);
    }
}

sf::FloatRect Circle::getBoundingBox() const {
    float r = baseVertices.empty() ? 0 : std::sqrt(baseVertices[0].x*baseVertices[0].x + baseVertices[0].y*baseVertices[0].y) * scaleFactor;
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