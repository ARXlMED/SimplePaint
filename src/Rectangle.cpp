#include "Rectangle.hpp"
#include <algorithm>

Rectangle::Rectangle(float width, float height, const sf::Color& color,
                     const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), baseWidth(width), baseHeight(height) {}

std::vector<sf::Vector2f> Rectangle::getLocalVertices() const {
    float w = baseWidth * scaleFactor;
    float h = baseHeight * scaleFactor;
    return {
        {-w/2, -h/2},
        { w/2, -h/2},
        { w/2,  h/2},
        {-w/2,  h/2}
    };
}

std::vector<sf::Vector2f> Rectangle::getGlobalVertices() const {
    auto local = getLocalVertices();
    std::vector<sf::Vector2f> global;
    for (auto& v : local) global.push_back(position + v);
    return global;
}

void Rectangle::draw(sf::RenderWindow& window) const {
    auto verts = getGlobalVertices();
    int n = verts.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        drawThickLine(window, verts[i], verts[j], thicknesses[i], sideColors[i]);
    }
    for (int i = 0; i < n; ++i) {
        float r = std::max(thicknesses[(i + n - 1) % n], thicknesses[i]) / 2.f;
        drawVertexCircle(window, verts[i], r, sideColors[i]);
    }
}

sf::FloatRect Rectangle::getBoundingBox() const {
    auto verts = getLocalVertices();
    float minX = verts[0].x, maxX = verts[0].x;
    float minY = verts[0].y, maxY = verts[0].y;
    for (auto& v : verts) {
        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);
        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }
    float maxThick = *std::max_element(thicknesses.begin(), thicknesses.end());
    return sf::FloatRect(position.x + minX - maxThick/2,
                         position.y + minY - maxThick/2,
                         (maxX - minX) + maxThick,
                         (maxY - minY) + maxThick);
}

bool Rectangle::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Rectangle::scale(float factor) {
    Figure::scale(factor);
}

void Rectangle::setScale(float factor) {
    Figure::setScale(factor);
}