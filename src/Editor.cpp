#include "Editor.hpp"
#include <algorithm>

Editor::Editor() : selectedFigure(nullptr), dragging(false) {}

void Editor::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        selectedFigure = nullptr;
        for (auto it = figures.rbegin(); it != figures.rend(); ++it) {
            if ((*it)->contains(mouse)) {
                selectedFigure = it->get();
                dragOffset = selectedFigure->getPosition() - mouse;
                dragging = true;
                break;
            }
        }
    }
    else if (event.type == sf::Event::MouseMoved && dragging && selectedFigure) {
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        selectedFigure->setPosition(mouse + dragOffset);
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
             event.mouseButton.button == sf::Mouse::Left) {
        dragging = false;
    }
}

void Editor::draw(sf::RenderWindow& window) {
    for (auto& fig : figures) fig->draw(window);
    if (selectedFigure) {
        sf::FloatRect bounds = selectedFigure->getBoundingBox();
        sf::RectangleShape rect({bounds.width, bounds.height});
        rect.setPosition(bounds.left, bounds.top);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Red);
        rect.setOutlineThickness(1);
        window.draw(rect);
    }
}

void Editor::addFigure(std::unique_ptr<Figure> fig) {
    figures.push_back(std::move(fig));
}

void Editor::removeSelected() {
    if (!selectedFigure) return;
    auto it = std::find_if(figures.begin(), figures.end(),
        [this](const std::unique_ptr<Figure>& f) { return f.get() == selectedFigure; });
    if (it != figures.end()) {
        figures.erase(it);
        selectedFigure = nullptr;
    }
}

void Editor::handleScale(float delta) {
    if (selectedFigure) {
        float newScale = selectedFigure->getScale() * (1.0f + delta * 0.1f);
        if (newScale > 0.1f && newScale < 5.0f)
            selectedFigure->setScale(newScale);
    }
}

Figure* Editor::getSelected() const {
    return selectedFigure;
}