#pragma once
#include "AbstractFigure.hpp"
#include <vector>
#include <memory>

class Editor {
public:
    Editor();
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void addFigure(std::unique_ptr<AbstractFigure> fig);
    void removeSelected();
    AbstractFigure* getSelected() const;
    void handleScale(float delta);

private:
    std::vector<std::unique_ptr<AbstractFigure>> figures;
    AbstractFigure* selectedFigure;
    sf::Vector2f dragOffset;
    bool dragging;
};