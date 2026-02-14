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
    sf::Color getColor() const;
    const std::vector<float>& getThicknesses() const;

protected:
    sf::Vector2f position;
    float scaleFactor;
    sf::Color lineColor;
    std::vector<float> thicknesses;

    void drawThickLine(sf::RenderWindow& window, const sf::Vector2f& A,
                       const sf::Vector2f& B, float thickness,
                       const sf::Color& color) const;
    void drawVertexCircle(sf::RenderWindow& window, const sf::Vector2f& vertex,
                          float radius, const sf::Color& color) const;
};