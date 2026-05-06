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
    void setSelected(AbstractFigure* fig);
    void handleScale(float delta);

    AbstractFigure* findFigureAt(const sf::Vector2f& point);
    bool removeFigure(AbstractFigure* fig);
    std::unique_ptr<AbstractFigure> releaseFigure(AbstractFigure* fig);
    size_t getFigureCount() const;
    AbstractFigure* getFigure(size_t index);
    const std::vector<std::unique_ptr<AbstractFigure>>& getFigures() const;

private:
    std::vector<std::unique_ptr<AbstractFigure>> figures;
    AbstractFigure* selectedFigure;
    sf::Vector2f dragOffset;
    bool dragging;
};