#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Figure {
public:
    Figure(const sf::Color& color, const std::vector<float>& thicknesses);
    virtual ~Figure() = default;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual bool contains(const sf::Vector2f& point) const = 0;
    virtual sf::FloatRect getBoundingBox() const = 0;
    virtual void move(const sf::Vector2f& offset);
    virtual void scale(float factor);
    virtual void setScale(float factor);

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    float getScale() const;
    const std::vector<float>& getThicknesses() const;

    void setSideColor(int index, sf::Color color);
    sf::Color getSideColor(int index) const;
    const std::vector<sf::Color>& getSideColors() const;

    void setFillColor(sf::Color color);
    sf::Color getFillColor() const;
    void setFilled(bool filled);
    bool isFilled() const;

    // Новые методы для вершин и точки привязки
    size_t getVertexCount() const { return baseVertices.size(); }
    sf::Vector2f getLocalVertex(size_t index) const { return baseVertices[index]; }
    sf::Vector2f getGlobalVertex(size_t index) const { return position + baseVertices[index] * scaleFactor; }
    void setLocalVertex(size_t index, const sf::Vector2f& pos) { if (index < baseVertices.size()) baseVertices[index] = pos; }

    sf::Vector2f getLocalPivot() const { return pivot; }
    sf::Vector2f getGlobalPivot() const { return position + pivot * scaleFactor; }
    void setLocalPivot(const sf::Vector2f& p) { pivot = p; }
    void movePivot(const sf::Vector2f& delta) { pivot += delta; }

protected:
    sf::Vector2f position;
    float scaleFactor;
    std::vector<float> thicknesses;
    std::vector<sf::Color> sideColors;
    sf::Color fillColor;
    bool filled;
    std::vector<sf::Vector2f> baseVertices; // локальные координаты вершин при scale=1
    sf::Vector2f pivot; // локальные координаты точки привязки относительно центра

    void drawThickLine(sf::RenderWindow& window, const sf::Vector2f& A,
                       const sf::Vector2f& B, float thickness,
                       const sf::Color& color) const;
    void drawVertexCircle(sf::RenderWindow& window, const sf::Vector2f& vertex,
                          float radius, const sf::Color& color) const;
};