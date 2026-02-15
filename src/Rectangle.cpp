#include "Rectangle.hpp"
#include <cmath>
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

    // Векторы для внутренних точек на каждой стороне
    std::vector<sf::Vector2f> inner_start(n), inner_end(n);

    // Вычисляем внутренние точки для каждой стороны
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sf::Vector2f dir = verts[j] - verts[i];
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 1e-6) continue;
        dir /= len;
        // Внутренняя нормаль (для обхода по часовой стрелке в SFML)
        sf::Vector2f norm_in(-dir.y, dir.x);
        float thick = thicknesses[i];
        sf::Vector2f offset = norm_in * (thick / 2.f);

        inner_start[i] = verts[i] + offset; // точка у начала стороны
        inner_end[i] = verts[j] + offset;   // точка у конца стороны
    }

    // Рисуем стороны
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sf::ConvexShape quad;
        quad.setPointCount(4);
        quad.setPoint(0, verts[i]);       // внешняя начало
        quad.setPoint(1, verts[j]);       // внешняя конец
        quad.setPoint(2, inner_end[i]);   // внутренняя конец
        quad.setPoint(3, inner_start[i]); // внутренняя начало
        quad.setFillColor(sideColors[i]);
        window.draw(quad);
    }

    // Заполняем углы
    for (int i = 0; i < n; ++i) {
        int prev = (i + n - 1) % n;
        sf::ConvexShape tri;
        tri.setPointCount(3);
        tri.setPoint(0, verts[i]);
        tri.setPoint(1, inner_end[prev]);  // внутренняя точка предыдущей стороны
        tri.setPoint(2, inner_start[i]);   // внутренняя точка текущей стороны
        tri.setFillColor(sideColors[i]);
        window.draw(tri);
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