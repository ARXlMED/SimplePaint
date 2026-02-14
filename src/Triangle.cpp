#include "Triangle.hpp"
#include <cmath>
#include <algorithm>

Triangle::Triangle(float side, const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), baseSide(side) {}

std::vector<sf::Vector2f> Triangle::getLocalVertices() const {
    float R = baseSide / std::sqrt(3.f) * scaleFactor;
    std::vector<sf::Vector2f> local(3);
    local[0] = {0.f, -R};
    local[1] = {R * 0.866f, R * 0.5f};
    local[2] = {-R * 0.866f, R * 0.5f};
    return local;
}

std::vector<sf::Vector2f> Triangle::getGlobalVertices() const {
    auto local = getLocalVertices();
    std::vector<sf::Vector2f> global;
    for (auto& v : local) global.push_back(position + v);
    return global;
}

void Triangle::draw(sf::RenderWindow& window) const {
    auto verts = getGlobalVertices();
    for (int i = 0; i < 3; ++i) {
        int j = (i + 1) % 3;
        drawThickLine(window, verts[i], verts[j], thicknesses[i], lineColor);
    }
    for (int i = 0; i < 3; ++i) {
        float r = std::max(thicknesses[(i+2)%3], thicknesses[i]) / 2.f;
        drawVertexCircle(window, verts[i], r, lineColor);
    }
}

sf::FloatRect Triangle::getBoundingBox() const {
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

bool Triangle::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Triangle::scale(float factor) {
    Figure::scale(factor);
}

void Triangle::setScale(float factor) {
    Figure::setScale(factor);
}