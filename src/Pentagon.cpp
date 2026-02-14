#include "Pentagon.hpp"
#include <cmath>
#include <algorithm>

Pentagon::Pentagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), baseRadius(radius) {}

std::vector<sf::Vector2f> Pentagon::getLocalVertices() const {
    std::vector<sf::Vector2f> local;
    float R = baseRadius * scaleFactor;
    for (int i = 0; i < 5; ++i) {
        float angle = i * 2 * M_PI / 5 - M_PI/2;
        local.emplace_back(R * std::cos(angle), R * std::sin(angle));
    }
    return local;
}

std::vector<sf::Vector2f> Pentagon::getGlobalVertices() const {
    auto local = getLocalVertices();
    std::vector<sf::Vector2f> global;
    for (auto& v : local) global.push_back(position + v);
    return global;
}

void Pentagon::draw(sf::RenderWindow& window) const {
    auto verts = getGlobalVertices();
    for (int i = 0; i < 5; ++i) {
        int j = (i + 1) % 5;
        drawThickLine(window, verts[i], verts[j], thicknesses[i], sideColors[i]);
    }
    for (int i = 0; i < 5; ++i) {
        float r = std::max(thicknesses[(i+4)%5], thicknesses[i]) / 2.f;
        drawVertexCircle(window, verts[i], r, sideColors[i]);
    }
}

sf::FloatRect Pentagon::getBoundingBox() const {
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

bool Pentagon::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Pentagon::scale(float factor) {
    Figure::scale(factor);
}

void Pentagon::setScale(float factor) {
    Figure::setScale(factor);
}