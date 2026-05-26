#include "PolylineFigure.hpp"
#include <cmath>
#include <algorithm>

PolylineFigure::PolylineFigure(const sf::Color& outlineColor, const std::vector<float>& thicknesses)
    : thicknesses(thicknesses) {
    sideColors.assign(thicknesses.size(), outlineColor);
}

std::unique_ptr<AbstractFigure> PolylineFigure::clone() const {
    auto newFig = std::make_unique<PolylineFigure>(
        sideColors.empty() ? sf::Color::White : sideColors[0],
        thicknesses
    );
    newFig->vertices = vertices;
    newFig->sideColors = sideColors;
    newFig->position = position;
    newFig->scaleFactor = scaleFactor;
    newFig->fillColor = fillColor;
    newFig->filled = filled;
    newFig->pivot = pivot;
    return newFig;
}

void PolylineFigure::draw(sf::RenderWindow& window) const {
    if (vertices.size() < 2) return;

    std::vector<sf::Vector2f> global;
    for (const auto& v : vertices)
        global.push_back(position + v * scaleFactor);
    size_t n = global.size();

    if (filled && n >= 3) {
        sf::ConvexShape fillShape;
        fillShape.setPointCount(n);
        for (size_t i = 0; i < n; ++i)
            fillShape.setPoint(i, global[i]);
        fillShape.setFillColor(fillColor);
        window.draw(fillShape);
    }

    std::vector<sf::Vector2f> outPoint(n), inPoint(n);
    for (size_t i = 0; i < n; ++i) {
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;

        sf::Vector2f V = global[i];
        sf::Vector2f P = global[prev];
        sf::Vector2f N = global[next];

        float t_prev = thicknesses[prev];
        float t_next = thicknesses[i];

        sf::Vector2f dir_prev = V - P;
        sf::Vector2f dir_next = N - V;
        float len_prev = std::sqrt(dir_prev.x * dir_prev.x + dir_prev.y * dir_prev.y);
        float len_next = std::sqrt(dir_next.x * dir_next.x + dir_next.y * dir_next.y);
        if (len_prev < 1e-6 || len_next < 1e-6) {
            outPoint[i] = V;
            inPoint[i] = V;
            continue;
        }
        dir_prev /= len_prev;
        dir_next /= len_next;

        sf::Vector2f norm_out_prev(-dir_prev.y, dir_prev.x);
        sf::Vector2f norm_out_next(-dir_next.y, dir_next.x);
        sf::Vector2f norm_in_prev = -norm_out_prev;
        sf::Vector2f norm_in_next = -norm_out_next;

        sf::Vector2f A_out = V + norm_out_prev * (t_prev / 2.f);
        sf::Vector2f B_out = V + norm_out_next * (t_next / 2.f);
        sf::Vector2f A_in  = V + norm_in_prev * (t_prev / 2.f);
        sf::Vector2f B_in  = V + norm_in_next * (t_next / 2.f);

        float det = dir_prev.x * dir_next.y - dir_prev.y * dir_next.x;
        if (std::abs(det) < 1e-6) {
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

    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
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

bool PolylineFigure::contains(const sf::Vector2f& point) const {
    return getBoundingBox().contains(point);
}

sf::FloatRect PolylineFigure::getBoundingBox() const {
    if (vertices.empty()) return {0,0,0,0};
    std::vector<sf::Vector2f> global;
    for (const auto& v : vertices)
        global.push_back(position + v * scaleFactor);
    float minX = global[0].x, maxX = global[0].x;
    float minY = global[0].y, maxY = global[0].y;
    for (auto& v : global) {
        minX = std::min(minX, v.x);
        maxX = std::max(maxX, v.x);
        minY = std::min(minY, v.y);
        maxY = std::max(maxY, v.y);
    }
    float maxThick = thicknesses.empty() ? 0 : *std::max_element(thicknesses.begin(), thicknesses.end());
    return sf::FloatRect(minX - maxThick/2, minY - maxThick/2,
                         (maxX - minX) + maxThick, (maxY - minY) + maxThick);
}

void PolylineFigure::setThickness(size_t index, float thick) {
    if (index < thicknesses.size())
        thicknesses[index] = thick;
}

void PolylineFigure::setSideColor(size_t index, sf::Color color) {
    if (index < sideColors.size())
        sideColors[index] = color;
}

sf::Color PolylineFigure::getSideColor(size_t index) const {
    if (index < sideColors.size())
        return sideColors[index];
    return sf::Color::White;
}

void PolylineFigure::addVertex(const sf::Vector2f& pos) {
    vertices.push_back(pos);
    if (vertices.size() > 1) {
        thicknesses.push_back(thicknesses.back());
        sideColors.push_back(sideColors.back());
    }
}

void PolylineFigure::removeVertex(size_t index) {
    if (index >= vertices.size()) return;
    vertices.erase(vertices.begin() + index);
    if (index < thicknesses.size()) {
        thicknesses.erase(thicknesses.begin() + index);
        sideColors.erase(sideColors.begin() + index);
    }
}
/*
void PolylineFigure::serialize(std::ostream& out) const {
    AbstractFigure::serialize(out);
    size_t n = vertices.size();
    out << n << '\n';
    for (size_t i = 0; i < n; ++i) {
        out << vertices[i].x << ' ' << vertices[i].y << ' ';
    }
    out << '\n';
    for (size_t i = 0; i < n; ++i) {
        out << thicknesses[i] << ' ';
    }
    out << '\n';
    for (size_t i = 0; i < n; ++i) {
        out << (int)sideColors[i].r << ' ' << (int)sideColors[i].g << ' ' << (int)sideColors[i].b << ' ';
    }
    out << '\n';
}

void PolylineFigure::deserialize(std::istream& in) {
    AbstractFigure::deserialize(in);
    size_t n;
    in >> n;
    vertices.resize(n);
    thicknesses.resize(n);
    sideColors.resize(n);

    for (size_t i = 0; i < n; ++i) {
        in >> vertices[i].x >> vertices[i].y;
    }
    for (size_t i = 0; i < n; ++i) {
        in >> thicknesses[i];
    }
    for (size_t i = 0; i < n; ++i) {
        int r,g,b;
        in >> r >> g >> b;
        sideColors[i] = sf::Color(r,g,b);
    }
}
*/


void PolylineFigure::serialize(std::ostream& out) const {
    AbstractFigure::serialize(out);
    size_t n = vertices.size();
    out << n << '\n';
    for (size_t i = 0; i < n; ++i) 
        out << vertices[i].x << ' ' << vertices[i].y << ' ';
    out << '\n';
    
    // Записываем n толщин и n цветов (для замкнутого контура)
    for (size_t i = 0; i < n; ++i) {
        float t = (i < thicknesses.size()) ? thicknesses[i] : 2.0f; // запасной вариант
        out << t << ' ';
    }
    out << '\n';
    for (size_t i = 0; i < n; ++i) {
        sf::Color c = (i < sideColors.size()) ? sideColors[i] : sf::Color::White;
        out << (int)c.r << ' ' << (int)c.g << ' ' << (int)c.b << ' ';
    }
    out << '\n';
}

void PolylineFigure::deserialize(std::istream& in) {
    AbstractFigure::deserialize(in); // Сначала читаем общие данные

    size_t n;
    if (!(in >> n)) return;

    vertices.resize(n);
    thicknesses.resize(n);
    sideColors.resize(n);

    // Читаем координаты всех вершин
    for (size_t i = 0; i < n; ++i) {
        in >> vertices[i].x >> vertices[i].y;
    }

    // Читаем толщины
    for (size_t i = 0; i < n; ++i) {
        in >> thicknesses[i];
    }

    // Читаем цвета сторон
    for (size_t i = 0; i < n; ++i) {
        int r, g, b;
        in >> r >> g >> b;
        sideColors[i] = sf::Color(r, g, b);
    }
}