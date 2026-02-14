#include "Trapezoid.hpp"
#include <algorithm>

Trapezoid::Trapezoid(float topBase, float bottomBase, float height,
                     const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), topBase(topBase), bottomBase(bottomBase), baseHeight(height) {}

std::vector<sf::Vector2f> Trapezoid::getLocalVertices() const {
    float wTop = topBase * scaleFactor;
    float wBottom = bottomBase * scaleFactor;
    float h = baseHeight * scaleFactor;
    return {
        { -wBottom/2, -h/2 },
        {  wBottom/2, -h/2 },
        {  wTop/2,     h/2 },
        { -wTop/2,     h/2 }
    };
}

std::vector<sf::Vector2f> Trapezoid::getGlobalVertices() const {
    auto local = getLocalVertices();
    std::vector<sf::Vector2f> global;
    for (auto& v : local) global.push_back(position + v);
    return global;
}

void Trapezoid::draw(sf::RenderWindow& window) const {
    auto verts = getGlobalVertices();
    for (int i = 0; i < 4; ++i) {
        int j = (i + 1) % 4;
        drawThickLine(window, verts[i], verts[j], thicknesses[i], sideColors[i]);
    }
    for (int i = 0; i < 4; ++i) {
        float r = std::max(thicknesses[(i+3)%4], thicknesses[i]) / 2.f;
        drawVertexCircle(window, verts[i], r, sideColors[i]);
    }
}

sf::FloatRect Trapezoid::getBoundingBox() const {
    auto verts = getGlobalVertices();
    float minX = verts[0].x, maxX = verts[0].x;
    float minY = verts[0].y, maxY = verts[0].y;
    for (auto& v : verts) {
        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);
        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }
    float maxThick = *std::max_element(thicknesses.begin(), thicknesses.end());
    return sf::FloatRect(minX - maxThick/2, minY - maxThick/2,
                         (maxX - minX) + maxThick, (maxY - minY) + maxThick);
}

bool Trapezoid::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Trapezoid::scale(float factor) {
    Figure::scale(factor);
}

void Trapezoid::setScale(float factor) {
    Figure::setScale(factor);
}