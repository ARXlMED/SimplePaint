#pragma once
#include "AbstractFigure.hpp"

class PolylineFigure : public AbstractFigure {
public:
    PolylineFigure(const sf::Color& outlineColor, const std::vector<float>& thicknesses);
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    std::unique_ptr<AbstractFigure> clone() const override;

    void setThickness(size_t index, float thick);
    void setSideColor(size_t index, sf::Color color);
    sf::Color getSideColor(size_t index) const;
    const std::vector<float>& getThicknesses() const { return thicknesses; }
    const std::vector<sf::Color>& getSideColors() const { return sideColors; }

    void addVertex(const sf::Vector2f& pos) override;
    void removeVertex(size_t index) override;

protected:
    std::vector<float> thicknesses;
    std::vector<sf::Color> sideColors;
};