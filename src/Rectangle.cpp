#include "Rectangle.hpp"
#include <cmath>
#include <algorithm>

#include "Rectangle.hpp"
#include <cmath>
#include <algorithm>

Rectangle::Rectangle(float width, float height, const sf::Color& color,
                     const std::vector<float>& thicknesses)
    : Figure(color, thicknesses) {
    baseVertices = {
        {-width/2, -height/2},
        { width/2, -height/2},
        { width/2,  height/2},
        {-width/2,  height/2}
    };
}

void Rectangle::draw(sf::RenderWindow& window) const {
    // Глобальные вершины с учётом масштаба
    std::vector<sf::Vector2f> verts;
    for (const auto& v : baseVertices)
        verts.push_back(position + v * scaleFactor);
    int n = verts.size();

    // Заливка (если включена)
    if (filled) {
        sf::ConvexShape fillShape;
        fillShape.setPointCount(n);
        for (int i = 0; i < n; ++i)
            fillShape.setPoint(i, verts[i]);
        fillShape.setFillColor(fillColor);
        window.draw(fillShape);
    }

    // Вычисляем точки пересечения для каждой вершины
    std::vector<sf::Vector2f> outPoint(n), inPoint(n);
    for (int i = 0; i < n; ++i) {
        int prev = (i + n - 1) % n;
        int next = (i + 1) % n;

        sf::Vector2f V = verts[i];
        sf::Vector2f P = verts[prev];
        sf::Vector2f N = verts[next];

        float t_prev = thicknesses[prev];
        float t_next = thicknesses[i];

        // Направления вдоль сторон (от предыдущей к текущей и от текущей к следующей)
        sf::Vector2f dir_prev = V - P;  // от P к V
        sf::Vector2f dir_next = N - V;  // от V к N
        float len_prev = std::sqrt(dir_prev.x * dir_prev.x + dir_prev.y * dir_prev.y);
        float len_next = std::sqrt(dir_next.x * dir_next.x + dir_next.y * dir_next.y);
        if (len_prev < 1e-6 || len_next < 1e-6) {
            outPoint[i] = V;
            inPoint[i] = V;
            continue;
        }
        dir_prev /= len_prev;
        dir_next /= len_next;

        // Внешние нормали (поворот на -90°)
        sf::Vector2f norm_out_prev(-dir_prev.y, dir_prev.x);
        sf::Vector2f norm_out_next(-dir_next.y, dir_next.x);
        sf::Vector2f norm_in_prev = -norm_out_prev;
        sf::Vector2f norm_in_next = -norm_out_next;

        // Точки на внешних и внутренних линиях у вершины
        sf::Vector2f A_out = V + norm_out_prev * (t_prev / 2.f);
        sf::Vector2f B_out = V + norm_out_next * (t_next / 2.f);
        sf::Vector2f A_in  = V + norm_in_prev * (t_prev / 2.f);
        sf::Vector2f B_in  = V + norm_in_next * (t_next / 2.f);

        float det = dir_prev.x * dir_next.y - dir_prev.y * dir_next.x;
        if (std::abs(det) < 1e-6) {
            // Почти параллельны – берём среднюю точку
            outPoint[i] = (A_out + B_out) / 2.f;
            inPoint[i]  = (A_in + B_in) / 2.f;
        } else {
            sf::Vector2f delta_out = B_out - A_out;
            float u_out = (delta_out.x * dir_next.y - delta_out.y * dir_next.x) / det;
            outPoint[i] = A_out + u_out * dir_prev;

            sf::Vector2f delta_in = B_in - A_in;
            float u_in = (delta_in.x * dir_next.y - delta_in.y * dir_next.x) / det;
            inPoint[i] = A_in + u_in * dir_prev;
        }
    }

    // Рисуем стороны как четырёхугольники (трапеции)
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        sf::ConvexShape quad;
        quad.setPointCount(4);
        quad.setPoint(0, outPoint[i]);
        quad.setPoint(1, outPoint[j]);
        quad.setPoint(2, inPoint[j]);
        quad.setPoint(3, inPoint[i]);
        quad.setFillColor(sideColors[i]);
        window.draw(quad);
    }
}

sf::FloatRect Rectangle::getBoundingBox() const {
    std::vector<sf::Vector2f> global;
    for (const auto& v : baseVertices)
        global.push_back(position + v * scaleFactor);
    float minX = global[0].x, maxX = global[0].x;
    float minY = global[0].y, maxY = global[0].y;
    for (auto& v : global) {
        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);
        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }
    float maxThick = *std::max_element(thicknesses.begin(), thicknesses.end());
    return sf::FloatRect(minX - maxThick/2, minY - maxThick/2,
                         (maxX - minX) + maxThick, (maxY - minY) + maxThick);
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