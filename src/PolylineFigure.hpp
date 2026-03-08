#pragma once
#include "AbstractFigure.hpp"

class PolylineFigure : public AbstractFigure {
public:
    PolylineFigure(const sf::Color& outlineColor, const std::vector<float>& thicknesses);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;

    void setThickness(size_t index, float thick);
    void setSideColor(size_t index, sf::Color color);
    const std::vector<float>& getThicknesses() const { return thicknesses; }
    const std::vector<sf::Color>& getSideColors() const { return sideColors; }

    void addVertex(const sf::Vector2f& pos) override;
    void removeVertex(size_t index) override;

    sf::Color getSideColor(size_t index) const;

protected:
    std::vector<float> thicknesses;   // для каждого ребра (количество = количество вершин для замкнутой)
    std::vector<sf::Color> sideColors;
};