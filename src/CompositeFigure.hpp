#pragma once
#include "AbstractFigure.hpp"
#include <vector>
#include <memory>

class CompositeFigure : public AbstractFigure {
public:
    CompositeFigure();
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;

    void addFigure(std::unique_ptr<AbstractFigure> fig, const sf::Vector2f& localPos);
    void removeFigure(size_t index);
    size_t getFigureCount() const { return children.size(); }
    AbstractFigure* getFigure(size_t index) { return children[index].figure.get(); }

private:
    struct Child {
        std::unique_ptr<AbstractFigure> figure;
        sf::Vector2f localOffset; // относительно центра композита
    };
    std::vector<Child> children;
};