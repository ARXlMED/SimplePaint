#include "Hexagon.hpp"
#include <cmath>
#include <algorithm>

Hexagon::Hexagon(float radius, const sf::Color& color, const std::vector<float>& thicknesses)
    : Figure(color, thicknesses), baseRadius(radius) {}

std::vector<sf::Vector2f> Hexagon::getLocalVertices() const {
    std::vector<sf::Vector2f> local;
    float R = baseRadius * scaleFactor;
    for (int i = 0; i < 6; ++i) {
        float angle = i * 2 * M_PI / 6 - M_PI/2;
        local.emplace_back(R * std::cos(angle), R * std::sin(angle));
    }
    return local;
}

std::vector<sf::Vector2f> Hexagon::getGlobalVertices() const {
    auto local = getLocalVertices();
    std::vector<sf::Vector2f> global;
    for (auto& v : local) global.push_back(position + v);
    return global;
}

void Hexagon::draw(sf::RenderWindow& window) const {
    auto verts = getGlobalVertices();
    int n = verts.size();

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sf::Vector2f dir = verts[j] - verts[i];
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 1e-6) continue;
        dir /= len;
        sf::Vector2f norm(-dir.y, dir.x);
        float thick = thicknesses[i];
        sf::Vector2f offset = norm * (thick / 2.f);

        sf::Vector2f A_in = verts[i] - offset;
        sf::Vector2f B_in = verts[j] - offset;

        sf::ConvexShape quad;
        quad.setPointCount(4);
        quad.setPoint(0, verts[i]);
        quad.setPoint(1, verts[j]);
        quad.setPoint(2, B_in);
        quad.setPoint(3, A_in);
        quad.setFillColor(sideColors[i]);
        window.draw(quad);
    }

    for (int i = 0; i < n; ++i) {
        int prev = (i + n - 1) % n;
        sf::Vector2f V = verts[i];

        sf::Vector2f dir_prev = verts[i] - verts[prev];
        float len_prev = std::sqrt(dir_prev.x * dir_prev.x + dir_prev.y * dir_prev.y);
        if (len_prev < 1e-6) continue;
        dir_prev /= len_prev;
        sf::Vector2f norm_prev(-dir_prev.y, dir_prev.x);
        float thick_prev = thicknesses[prev];
        sf::Vector2f P_in = V - norm_prev * (thick_prev / 2.f);

        sf::Vector2f dir_next = verts[(i + 1) % n] - V;
        float len_next = std::sqrt(dir_next.x * dir_next.x + dir_next.y * dir_next.y);
        if (len_next < 1e-6) continue;
        dir_next /= len_next;
        sf::Vector2f norm_next(-dir_next.y, dir_next.x);
        float thick_next = thicknesses[i];
        sf::Vector2f N_in = V - norm_next * (thick_next / 2.f);

        sf::ConvexShape tri;
        tri.setPointCount(3);
        tri.setPoint(0, V);
        tri.setPoint(1, P_in);
        tri.setPoint(2, N_in);
        tri.setFillColor(sideColors[i]);
        window.draw(tri);
    }
}

sf::FloatRect Hexagon::getBoundingBox() const {
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

bool Hexagon::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

void Hexagon::scale(float factor) {
    Figure::scale(factor);
}

void Hexagon::setScale(float factor) {
    Figure::setScale(factor);
}