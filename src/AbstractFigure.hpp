#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class AbstractFigure {
public:
    AbstractFigure();
    virtual ~AbstractFigure() = default;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual bool contains(const sf::Vector2f& point) const = 0;
    virtual sf::FloatRect getBoundingBox() const = 0;

    void move(const sf::Vector2f& offset);
    void scale(float factor);
    void setScale(float factor);
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    float getScale() const;

    void setFillColor(sf::Color color);
    sf::Color getFillColor() const;
    void setFilled(bool filled);
    bool isFilled() const;

    // Управление вершинами (локальные координаты)
    size_t getVertexCount() const;
    sf::Vector2f getLocalVertex(size_t index) const;
    sf::Vector2f getGlobalVertex(size_t index) const;
    void setLocalVertex(size_t index, const sf::Vector2f& pos);
    virtual void addVertex(const sf::Vector2f& pos);
    virtual void removeVertex(size_t index);

    // Точка привязки (локальная)
    sf::Vector2f getLocalPivot() const;
    sf::Vector2f getGlobalPivot() const;
    void setLocalPivot(const sf::Vector2f& p);
    void movePivot(const sf::Vector2f& delta);

protected:
    sf::Vector2f position;
    float scaleFactor;
    sf::Color fillColor;
    bool filled;
    std::vector<sf::Vector2f> vertices; // локальные координаты
    sf::Vector2f pivot;                  // локальный
};