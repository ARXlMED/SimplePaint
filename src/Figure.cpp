#include "Figure.hpp"
#include <cmath>
#include <algorithm>

Figure::Figure(const sf::Color& color, const std::vector<float>& thicknesses)
    : position(0, 0), scaleFactor(1.f), lineColor(color), thicknesses(thicknesses) {}

void Figure::move(const sf::Vector2f& offset) {
    position += offset;
}

void Figure::scale(float factor) {
    scaleFactor *= factor;
}

void Figure::setScale(float factor) {
    scaleFactor = factor;
}

void Figure::setPosition(const sf::Vector2f& pos) {
    position = pos;
}

sf::Vector2f Figure::getPosition() const {
    return position;
}

float Figure::getScale() const {
    return scaleFactor;
}

sf::Color Figure::getColor() const {
    return lineColor;
}

const std::vector<float>& Figure::getThicknesses() const {
    return thicknesses;
}

void Figure::drawThickLine(sf::RenderWindow& window, const sf::Vector2f& A, const sf::Vector2f& B, float thickness, const sf::Color& color) const {
    sf::Vector2f dir = B - A;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0) return;
    sf::Vector2f unitDir = dir / len;
    sf::Vector2f normal(-unitDir.y, unitDir.x);
    sf::Vector2f offset = normal * (thickness / 2.f);

    sf::ConvexShape polygon;
    polygon.setPointCount(4);
    polygon.setPoint(0, A - offset);
    polygon.setPoint(1, A + offset);
    polygon.setPoint(2, B + offset);
    polygon.setPoint(3, B - offset);
    polygon.setFillColor(color);
    window.draw(polygon);
}

void Figure::drawVertexCircle(sf::RenderWindow& window, const sf::Vector2f& vertex, float radius, const sf::Color& color) const {
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(vertex);
    circle.setFillColor(color);
    window.draw(circle);
}