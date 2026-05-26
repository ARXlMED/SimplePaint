#pragma once
#include "AbstractFigure.hpp"
#include <SFML/Graphics.hpp>
#include "FigureManager.hpp"

#define MAX_FIGURES 1000

class Editor {
public:
    Editor();
    ~Editor();

std::unique_ptr<AbstractFigure> extractFigure(AbstractFigure* fig);

    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void addFigure(AbstractFigure* fig);   // принимает владение сырым указателем
    void removeSelected();
    AbstractFigure* getSelected() const;
    void setSelected(AbstractFigure* fig);
    void handleScale(float delta);
    bool isSelectedValid() const;

    AbstractFigure* findFigureAt(const sf::Vector2f& point);
    bool removeFigure(AbstractFigure* fig);
    size_t getFigureCount() const;
    AbstractFigure* getFigure(size_t index);
    const AbstractFigure* const* getFigures() const;   // для доступа к массиву

    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

private:
    AbstractFigure* figures[MAX_FIGURES];
    int figureCount = 0;
    AbstractFigure* selectedFigure = nullptr;
    sf::Vector2f dragOffset;
    bool dragging = false;
};