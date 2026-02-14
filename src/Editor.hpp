#pragma once
#include "Figure.hpp"
#include <vector>
#include <memory>

class Editor {
public:
    Editor();
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void addFigure(std::unique_ptr<Figure> fig);
    void removeSelected();
    Figure* getSelected() const;
    void handleScale(float delta); 

private:
    std::vector<std::unique_ptr<Figure>> figures;
    Figure* selectedFigure;
    sf::Vector2f dragOffset;
    bool dragging;
};