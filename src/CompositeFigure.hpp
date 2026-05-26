#pragma once
#include "AbstractFigure.hpp"
#include <vector>
#include <memory>
#include "FigureManager.hpp"

class CompositeFigure : public AbstractFigure {
public:
    CompositeFigure();
    void draw(sf::RenderWindow& window) const override;
    bool contains(const sf::Vector2f& point) const override;
    sf::FloatRect getBoundingBox() const override;
    std::unique_ptr<AbstractFigure> clone() const override;

    void addFigure(std::unique_ptr<AbstractFigure> fig, const sf::Vector2f& localPos);
    void removeFigure(size_t index);
    size_t getFigureCount() const { return children.size(); }
    AbstractFigure* getFigure(size_t index) { return children[index].figure.get(); }
    std::unique_ptr<AbstractFigure> extractFigure(size_t index);  // новый метод

    size_t getChildCount() const { return children.size(); }
    AbstractFigure* getChild(size_t index) const { return children[index].figure.get(); }
    sf::Vector2f getChildOffset(size_t index) const { return children[index].localOffset; }


    void serialize(std::ostream& out) const override;
    void deserialize(std::istream& in) override;

private:
    struct Child {
        std::unique_ptr<AbstractFigure> figure;
        sf::Vector2f localOffset;
    };
    std::vector<Child> children;
};